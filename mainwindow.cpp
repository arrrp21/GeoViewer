#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QColorTransform>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>

#include <ios>
#include <cmath>

#include "GprData.hpp"
#include "ImageLabel.hpp"
#include "QImageWrapper.hpp"
#include "CommonImageTransformer.hpp"
#include "GpuImageTransformer.hpp"
#include "Visitor.hpp"
#include "fmt/format.h"
#include "Panel.hpp"
#include "Mask.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , imageLabel(new ImageLabel(scaleFactor))
    , panel(new Panel(this))
    , scrollArea(new QScrollArea)
    , layout(new QVBoxLayout)
    , centralWidget(new QWidget)
{
    ui->setupUi(this);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSave->setEnabled(false);

    panel->setVisible(false);

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);

    layout->addWidget(panel);
    layout->addWidget(scrollArea);

    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);

    connectSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scaleImage(float factor)
{
    const float newScaleFactor = scaleFactor * factor;

    if (newScaleFactor >= maxScaleFactor or newScaleFactor <= minScaleFactor)
        return;

    scaleFactor = newScaleFactor;
    QSize newSize = scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size();
    newSize.setHeight(newSize.height() + ImageLabel::bottomAxisHeight);
    newSize.setWidth(newSize.width() + ImageLabel::leftAxisWidth);
    imageLabel->resize(newSize);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, float factor)
{
    scrollBar->setValue(static_cast<int>(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::drawImage()
{
    imageLabel->setPixmap(QPixmap::fromImage(imageWrapper->getImage()));
    imageLabel->adjustSize();
}

void MainWindow::refreshImage()
{
    int x = scrollArea->horizontalScrollBar()->value();
    int y = scrollArea->verticalScrollBar()->value();
    drawImage();
    scaleImage(1.0f);
    scrollArea->horizontalScrollBar()->setValue(x);
    scrollArea->verticalScrollBar()->setValue(y);
}

void MainWindow::on_actionOpenTriggered(bool)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose file"), "./", tr("Profiles (*.asc);;Images (*.jpg *.png *.bmp)"));

    qDebug() << fileName;

    if (fileName.isNull())
        return;

    QFile file(fileName);

    std::variant<GprData, QString> maybeGprData = tryCreateGprData(file);

    std::visit(Visitor{
         [this](GprData& gprData) {
            imageWrapper = std::make_unique<QImageWrapper>(gprData);
            qDebug() << gprData;
            if (ui->actionGpuAcceleration->isChecked())
                imageTransformer = std::make_unique<image_transforming::GpuImageTransformer>(*imageWrapper);
            else
                imageTransformer = std::make_unique<image_transforming::CommonImageTransformer>(*imageWrapper);
            if (gprData.SCAN_DIRECTION == ScanDirection::L)
                imageTransformer->rotate90();
            imageLabel->setXStep(gprData.X_STEP);
            imageLabel->setYStep(gprData.RANGE / 2 * gprData.PROP_VEL / gprData.N_ACQ_SAMPLE);
            drawImage();
            ui->actionGainPanel->setChecked(true);
            panel->setImageHeight(imageWrapper->getImage().height());
            scrollArea->setVisible(true);
            ui->actionSave->setEnabled(true);
        },
        [](QString& error) {
            QMessageBox messageBox;
            messageBox.setText(error);
            messageBox.exec();
        }},
        maybeGprData);
}

void MainWindow::on_actionSaveTriggered(bool)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose file"), "./", tr("Images (*.jpg *.png *.bmp)"));
    qDebug() << fileName;

    if (fileName.isNull())
        return;

    imageWrapper->getImage().save(fileName);
}

void MainWindow::on_actionGainPannelToggled(bool checked)
{
    panel->setVisible(checked);
}

void MainWindow::on_actionRotate90Triggered(bool)
{
    imageTransformer->rotate90();
    drawImage();
}

void MainWindow::on_actionHighPassFilterTriggered(bool)
{
    image_transforming::details::Mask<int> highPassFilter{3, 3, {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}}};
    imageTransformer->applyFilter(image_transforming::Mask{highPassFilter});
    refreshImage();
}

void MainWindow::on_actionBackgroundRemovalTriggered(bool)
{
    imageTransformer->backgroundRemoval();
    refreshImage();
}

void MainWindow::on_actionTrimTopTriggered(bool)
{
    if (not isTopTrimmed)
    {
        imageTransformer->trimTop();
        refreshImage();
    }
    isTopTrimmed = true;
    ui->actionTrimTop->setEnabled(false);
}

void MainWindow::on_actionGpuAccelerationToggled(bool toggled)
{
    qDebug() << "on_actionGpuAccelerationToggled";
    if (imageTransformer and imageWrapper)
    {
        if (toggled)
            imageTransformer = std::make_unique<image_transforming::GpuImageTransformer>(*imageWrapper);
        else
            imageTransformer = std::make_unique<image_transforming::CommonImageTransformer>(*imageWrapper);
    }
}

void MainWindow::on_mouseWheelUsed(QPoint angleDelta)
{
    float factor = (angleDelta.y() > 0) ? (1.0f + scaleFactorStep) : (1.0f - scaleFactorStep);
    scaleImage(factor);
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
}

void MainWindow::on_mousePressedMoved(int x, int y)
{
    if (x != 0)
    {
        QScrollBar* horizontalScrollBar = scrollArea->horizontalScrollBar();
        int currentValue = horizontalScrollBar->value();
        horizontalScrollBar->setValue(currentValue + x);
    }

    if (y != 0)
    {
        QScrollBar* verticallScrollBar = scrollArea->verticalScrollBar();
        int currentValue = verticallScrollBar->value();
        verticallScrollBar->setValue(currentValue + y);
    };
}

void MainWindow::on_mouseMoved(int x, int y)
{
    x = std::lround(x / scaleFactor);
    y = std::lround(y / scaleFactor);

    GprData::DataType color = imageWrapper->getColor(x, y);
    ui->statusbar->showMessage(QString::fromStdString(
        fmt::format("x: {}  y: {}  color: {}  scale: {}  width: {}  height: {}",
                    x, y, color, scaleFactor, imageLabel->width(), imageLabel->height())));
}

void MainWindow::on_sliderGainChanged(int from, int to, double lowerGain, double upperGain)
{
    imageTransformer->gain(from, to, lowerGain, upperGain);
    drawImage();
}

void MainWindow::on_sliderRangeChanged(int from, int to)
{
    switch (operation)
    {
    case image_transforming::Operation::equalizeHist:
        imageTransformer->equalizeHistogram(from, to);
        refreshImage();
        return;
    case image_transforming::Operation::gain:
    {
        auto [lowerGain, upperGain] = panel->getLinearGainValues();
        imageTransformer->gain(from, to, lowerGain, upperGain);
        refreshImage();
        return;
    }
    default:
        return;
    }
}

void MainWindow::on_buttonEqualizeHistClicked(int from, int to)
{
    imageTransformer->equalizeHistogram(from, to);
    refreshImage();
}

void MainWindow::on_buttonResetClicked()
{
    imageWrapper->resetImage();
    scaleFactor = 1.0f;
    drawImage();
    isTopTrimmed = false;
    ui->actionTrimTop->setEnabled(true);
}

void MainWindow::on_buttonRotateClicked()
{
    imageTransformer->rotate90();
    imageLabel->setPixmap(QPixmap::fromImage(imageWrapper->getImage()));
    imageLabel->adjustSize();
    panel->setImageHeight(imageWrapper->getImage().height());
}

void MainWindow::on_rbEqualizeHistChecked()
{
    operation = image_transforming::Operation::equalizeHist;
}

void MainWindow::on_rbGainChecked()
{
    operation = image_transforming::Operation::gain;
}

void MainWindow::connectSignals()
{
    //      sender                        signal                                               receiver    slot
    connect(ui->actionOpen,               SIGNAL(triggered(bool)),                             this,  SLOT(on_actionOpenTriggered(bool))                       );
    connect(ui->actionSave,               SIGNAL(triggered(bool)),                             this,  SLOT(on_actionSaveTriggered(bool))                       );
    connect(ui->actionRotate90,           SIGNAL(triggered(bool)),                             this,  SLOT(on_actionRotate90Triggered(bool))                   );
    connect(ui->actionGainPanel,          SIGNAL(toggled(bool)),                               this,  SLOT(on_actionGainPannelToggled(bool))                   );
    connect(ui->actionHighPassFilter,     SIGNAL(triggered(bool)),                             this,  SLOT(on_actionHighPassFilterTriggered(bool))             );
    connect(ui->actionBackgroundRemoval,  SIGNAL(triggered(bool)),                             this,  SLOT(on_actionBackgroundRemovalTriggered(bool))          );
    connect(ui->actionTrimTop,            SIGNAL(triggered(bool)),                             this,  SLOT(on_actionTrimTopTriggered(bool))                    );
    connect(ui->actionGpuAcceleration,    SIGNAL(toggled(bool)),                               this,  SLOT(on_actionGpuAccelerationToggled(bool))              );
    connect(imageLabel,                   SIGNAL(mouseWheelUsed(QPoint)),                      this,  SLOT(on_mouseWheelUsed(QPoint))                          );
    connect(imageLabel,                   SIGNAL(mousePressedMoved(int,int)),                  this,  SLOT(on_mousePressedMoved(int,int))                      );
    connect(imageLabel,                   SIGNAL(mouseMoved(int,int)),                         this,  SLOT(on_mouseMoved(int,int))                             );
    connect(panel,                        SIGNAL(buttonRotateClicked()),                       this,  SLOT(on_buttonRotateClicked())                           );
    connect(panel,                        SIGNAL(sliderGainChanged(int, int, double, double)), this,  SLOT(on_sliderGainChanged(int, int, double, double))     );
    connect(panel,                        SIGNAL(buttonEqualizeHistClicked(int, int)),         this,  SLOT(on_buttonEqualizeHistClicked(int, int))             );
    connect(panel,                        SIGNAL(buttonResetClicked()),                        this,  SLOT(on_buttonResetClicked())                            );
    connect(panel,                        SIGNAL(rbEqualizeHistChecked()),                     this,  SLOT(on_rbEqualizeHistChecked())                         );
    connect(panel,                        SIGNAL(rbGainChecked()),                             this,  SLOT(on_rbGainChecked())                                 );
    connect(panel,                        SIGNAL(sliderRangeChanged(int, int)),                this,  SLOT(on_sliderRangeChanged(int, int))                    );
}
