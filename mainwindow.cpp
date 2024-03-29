#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include "Log.hpp"

#include "GprData.hpp"
#include "ImageLabel.hpp"
#include "QImageWrapper.hpp"
#include "CommonImageTransformer.hpp"
#include "GpuImageTransformer.hpp"
#include "Visitor.hpp"
#include "Panel.hpp"
#include "ChooseMaskDialog.hpp"
#include "Mask.hpp"
#include "StateMachine.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , imageLabel(new ImageLabel(scaleFactor))
    , panel(new Panel(this))
    , chooseMaskDialog(new ChooseMaskDialog(this))
    , scrollArea(new QScrollArea)
    , layout(new QVBoxLayout)
    , centralWidget(new QWidget)
{
    ui->setupUi(this);

    setShortcuts();

    panel->hide();

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->hide();

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

void MainWindow::enableMenuActions()
{
    toggleMenuActions(true);
}

void MainWindow::disableMenuActions()
{
    toggleMenuActions(false);
}

void MainWindow::toggleMenuActions(bool isEnabled)
{
    ui->actionSave->setEnabled(isEnabled);
    ui->actionClose->setEnabled(isEnabled);
    ui->menuEdit->setEnabled(isEnabled);
    ui->menuTools->setEnabled(isEnabled);
}

void MainWindow::askForSaveImage()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(
                this,
                "GeoViewer",
                "Do you want to save your image before closing?",
                QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        saveImage();
    }
}

void MainWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose file"), "./", tr("Images (*.jpg *.png *.bmp);;Plain Text(*.txt)"));
    qDebug() << fileName;

    if (fileName.isNull())
        return;

    if (fileName.endsWith(".txt"))
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QTextStream stream(&file);

        const ImageData& imageData = imageWrapper->getImageData();
        for (int row = 0; row < imageWrapper->height(); row++)
        {
            for (int col = 0; col < imageWrapper->width(); col++)
            {
                stream << static_cast<int>(imageData.at(row, col)) - static_cast<int>(std::numeric_limits<GprData::DataType>::max())/2 - 1 << " ";
            }
            stream << "\n";
        }
        file.close();
        return;
    }

    imageWrapper->getImage().save(fileName);
}

void MainWindow::closeImage()
{
    disableMenuActions();
    imageLabel->clearImage();
    stateMachine.reset();
    imageWrapper.reset();
    imageTransformer.reset();
    panel->hide();
    scrollArea->hide();
}

void MainWindow::scaleImage(float factor)
{
    scaleFactor = factor;
    QSize newSize = scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size();
    newSize.setHeight(newSize.height() + ImageLabel::bottomAxisHeight);
    newSize.setWidth(newSize.width() + ImageLabel::leftAxisWidth);
    imageLabel->resize(newSize);
}

void MainWindow::setTopTrimmed(bool topTrimmed)
{
    isTopTrimmed = topTrimmed;
    ui->actionTrimTop->setEnabled(not topTrimmed);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, float factor)
{
    scrollBar->setValue(static_cast<int>(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::rotate90()
{
    imageTransformer->rotate90();
    imageLabel->setPixmap(QPixmap::fromImage(imageWrapper->getImage()));
    imageLabel->adjustSize();
    panel->setImageHeight(imageWrapper->height());
}

void MainWindow::drawImage()
{
    imageLabel->showImage(imageWrapper->getImage());
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

    LOG_INFO("fileName: {}", fileName.toStdString());

    if (fileName.isNull())
        return;

    QFile file(fileName);

    std::variant<GprData, std::string> maybeGprData = tryCreateGprData(file);

    std::visit(Visitor{
         [this](GprData& gprData) {
            imageWrapper = std::make_unique<QImageWrapper>(gprData);
            qDebug() << gprData;
            if (ui->actionGpuAcceleration->isChecked())
                imageTransformer = std::make_unique<image_transforming::GpuImageTransformer>(*imageWrapper);
            else
                imageTransformer = std::make_unique<image_transforming::CommonImageTransformer>(*imageWrapper);
            if (gprData.SCAN_DIRECTION == ScanDirection::L)
            {
                imageTransformer->rotate90();
                imageWrapper->updatePreviousImage();
                imageWrapper->updateOriginalImage();
            }
            imageLabel->setXStep(gprData.X_STEP);
            imageLabel->setYStep(gprData.RANGE / 2 * gprData.PROP_VEL / gprData.N_ACQ_SAMPLE);
            drawImage();
            ui->actionEnablePanel->setChecked(true);
            setTopTrimmed(false);
            panel->setImageHeight(imageWrapper->height());
            panel->show();
            scrollArea->show();
            enableMenuActions();
            stateMachine = std::make_unique<StateMachine>();
            State state = createState();
            stateMachine->updateState(state);
        },
        [](std::string& error) {
            QMessageBox messageBox;
            messageBox.setText(QString::fromStdString(error));
            messageBox.exec();
        }},
        maybeGprData);
}

void MainWindow::on_actionSaveTriggered(bool)
{
    saveImage();
}

void MainWindow::on_actionCloseTriggered(bool)
{
    askForSaveImage();
    closeImage();
}

void MainWindow::on_actionExitTriggered(bool)
{
    if (imageWrapper)
    {
        askForSaveImage();
    }
    QApplication::quit();
}

void MainWindow::on_actionUndoTriggered(bool)
{
    std::optional<State> state = stateMachine->previousState();
    if (state)
    {
        restoreState(state.value());
    }
}

void MainWindow::on_actionRedoTriggered(bool)
{
    std::optional<State> state = stateMachine->nextState();
    if (state)
    {
        restoreState(state.value());
    }
}

void MainWindow::on_actionEnablePanelToggled(bool checked)
{
    panel->setVisible(checked);
}

void MainWindow::on_actionRotate90Triggered(bool)
{
    rotate90();
}

void MainWindow::on_actionApplyFilterTriggered(bool)
{
    if (chooseMaskDialog->exec() == QDialog::Accepted)
    {
        image_transforming::Mask mask = chooseMaskDialog->getMask();
        imageTransformer->applyFilter(mask);
        refreshImage();
        updateState();
    }
}

void MainWindow::on_actionBackgroundRemovalTriggered(bool)
{
    imageTransformer->backgroundRemoval();
    refreshImage();
    updateState();
}

void MainWindow::on_actionTrimTopTriggered(bool)
{
    if (not isTopTrimmed)
    {
        imageTransformer->trimTop();
        panel->setImageHeight(imageWrapper->height());
        refreshImage();
        setTopTrimmed(true);
        updateState();
    }
}

void MainWindow::on_actionGpuAccelerationToggled(bool toggled)
{
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
    float newScaleFactor = factor * scaleFactor;

    if (newScaleFactor < maxScaleFactor and newScaleFactor > minScaleFactor)
    {
        scaleImage(newScaleFactor);
        adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
        adjustScrollBar(scrollArea->verticalScrollBar(), factor);
    }
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

    int signalAmplitude = static_cast<int>(imageWrapper->getColor(x, y)) - std::numeric_limits<GprData::DataType>::max()/2 - 1;
    ui->statusbar->showMessage(QString::fromStdString(
        fmt::format("x: {}  y: {}  signal amplitude: {}  scale: {:.2f}  width: {}  height: {}",
                    x, y, signalAmplitude, scaleFactor, imageWrapper->width(), imageWrapper->height())));
}

void MainWindow::on_sliderGainChanged(int from, int to, double lowerGain, double upperGain)
{
    imageTransformer->gain(from, to, lowerGain, upperGain);
    refreshImage();
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

void MainWindow::on_buttonResetClicked()
{
    imageWrapper->resetImage();
    panel->setImageHeight(imageWrapper->height());
    panel->resetGainSliders();
    scaleFactor = 1.0f;
    drawImage();
    setTopTrimmed(false);
    updateState();
}

void MainWindow::on_buttonRotateClicked()
{
    rotate90();
    updateState();
}

void MainWindow::on_rbEqualizeHistChecked()
{
    operation = image_transforming::Operation::equalizeHist;
    auto [from, to] = panel->getSliderRangeValues();
    imageTransformer->equalizeHistogram(from, to);
    refreshImage();
}

void MainWindow::on_rbGainChecked()
{
    operation = image_transforming::Operation::gain;
    auto [from, to] = panel->getSliderRangeValues();
    auto [gainLower, gainUpper] = panel->getLinearGainValues();
    imageTransformer->gain(from, to, gainLower, gainUpper);
    refreshImage();
}

void MainWindow::on_buttonApplyClicked()
{
    if (operation != image_transforming::Operation::none)
    {
        imageTransformer->commitChanges(operation);
        updateState();
    }
}

void MainWindow::on_buttonCancelClicked()
{
    imageTransformer->commitChanges(operation);
    State state = stateMachine->latestState();
    restoreState(state);
    resetOperation();
}

State MainWindow::createState()
{
    auto [sliderRangeLowerValue, sliderRangeUpperValue] = panel->getSliderRangeValues();
    auto [sliderGainLowerValue, sliderGainUpperValue] = panel->getLinearGainSliderValues();

    return State{sliderRangeLowerValue,
        sliderRangeUpperValue,
        sliderGainLowerValue,
        sliderGainUpperValue,
        isTopTrimmed,
        imageWrapper->getImageData(),
        operation};
}

void MainWindow::updateState()
{
    State state = createState();
    stateMachine->updateState(state);
    imageWrapper->updatePreviousImage();
    resetOperation();
}

void MainWindow::restoreState(State& state)
{
    panel->restoreState(state);
    setTopTrimmed(state.isTopTrimmed);
    imageWrapper->setNewImage(std::move(state.imageData));
    imageWrapper->updatePreviousImage();
    panel->setImageHeight(imageWrapper->height());
    refreshImage();
}

void MainWindow::resetOperation()
{
    panel->uncheckRadioButton();
    operation = image_transforming::Operation::none;
}

void MainWindow::setShortcuts()
{
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionClose->setShortcut(QKeySequence::Close);
    ui->actionExit->setShortcut(QKeySequence::Quit);
}

void MainWindow::connectSignals()
{
    //      sender                        signal                                               receiver    slot
    connect(ui->actionOpen,               SIGNAL(triggered(bool)),                             this,  SLOT(on_actionOpenTriggered(bool))                       );
    connect(ui->actionSave,               SIGNAL(triggered(bool)),                             this,  SLOT(on_actionSaveTriggered(bool))                       );
    connect(ui->actionClose,              SIGNAL(triggered(bool)),                             this,  SLOT(on_actionCloseTriggered(bool))                      );
    connect(ui->actionExit,               SIGNAL(triggered(bool)),                             this,  SLOT(on_actionExitTriggered(bool))                       );
    connect(ui->actionUndo,               SIGNAL(triggered(bool)),                             this,  SLOT(on_actionUndoTriggered(bool))                       );
    connect(ui->actionRedo,               SIGNAL(triggered(bool)),                             this,  SLOT(on_actionRedoTriggered(bool))                       );
    connect(ui->actionRotate90,           SIGNAL(triggered(bool)),                             this,  SLOT(on_actionRotate90Triggered(bool))                   );
    connect(ui->actionEnablePanel,        SIGNAL(toggled(bool)),                               this,  SLOT(on_actionEnablePanelToggled(bool))                  );
    connect(ui->actionApplyFilter,        SIGNAL(triggered(bool)),                             this,  SLOT(on_actionApplyFilterTriggered(bool))                );
    connect(ui->actionBackgroundRemoval,  SIGNAL(triggered(bool)),                             this,  SLOT(on_actionBackgroundRemovalTriggered(bool))          );
    connect(ui->actionTrimTop,            SIGNAL(triggered(bool)),                             this,  SLOT(on_actionTrimTopTriggered(bool))                    );
    connect(ui->actionGpuAcceleration,    SIGNAL(toggled(bool)),                               this,  SLOT(on_actionGpuAccelerationToggled(bool))              );
    connect(imageLabel,                   SIGNAL(mouseWheelUsed(QPoint)),                      this,  SLOT(on_mouseWheelUsed(QPoint))                          );
    connect(imageLabel,                   SIGNAL(mousePressedMoved(int,int)),                  this,  SLOT(on_mousePressedMoved(int,int))                      );
    connect(imageLabel,                   SIGNAL(mouseMoved(int,int)),                         this,  SLOT(on_mouseMoved(int,int))                             );
    connect(panel,                        SIGNAL(buttonRotateClicked()),                       this,  SLOT(on_buttonRotateClicked())                           );
    connect(panel,                        SIGNAL(sliderGainChanged(int, int, double, double)), this,  SLOT(on_sliderGainChanged(int, int, double, double))     );
    connect(panel,                        SIGNAL(buttonResetClicked()),                        this,  SLOT(on_buttonResetClicked())                            );
    connect(panel,                        SIGNAL(rbEqualizeHistChecked()),                     this,  SLOT(on_rbEqualizeHistChecked())                         );
    connect(panel,                        SIGNAL(rbGainChecked()),                             this,  SLOT(on_rbGainChecked())                                 );
    connect(panel,                        SIGNAL(sliderRangeChanged(int, int)),                this,  SLOT(on_sliderRangeChanged(int, int))                    );
    connect(panel,                        SIGNAL(buttonApplyClicked()),                        this,  SLOT(on_buttonApplyClicked())                            );
    connect(panel,                        SIGNAL(buttonCancelClicked()),                       this,  SLOT(on_buttonCancelClicked())                           );
}

