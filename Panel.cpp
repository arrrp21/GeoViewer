#include "Panel.hpp"
#include "ui_Panel.h"

#include <QIntValidator>
#include <QDoubleValidator>

Panel::Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Panel)
{
    ui->setupUi(this);

    ui->sliderGainValueLower->setMinimum(std::lround(minGain * multiplier));
    ui->sliderGainValueLower->setMaximum(std::lround(maxGain * multiplier) - 1);
    ui->sliderGainValueLower->setValue(std::lround(defaultGainLower * multiplier));
    ui->labelGainValueLower->setText(QString::number(defaultGainLower));

    ui->sliderGainValueUpper->setMinimum(std::lround(minGain * multiplier) + 1);
    ui->sliderGainValueUpper->setMaximum(std::lround(maxGain * multiplier));
    ui->sliderGainValueUpper->setValue(std::lround(defaultGainUpper * multiplier));
    ui->labelGainValueUpper->setText(QString::number(defaultGainUpper));

    QPixmap pixmap("../GeoViewer/resources/rotate.png");
    QIcon icon(pixmap);
    ui->buttonRotate->setIcon(icon);
    ui->buttonRotate->setIconSize(QSize(24, 24));

    connectSignals();
}

void Panel::setImageHeight(int imageHeight)
{
    this->imageHeight = imageHeight;
    setHeightConstraints();
}

void Panel::setHeightConstraints()
{
    if (not imageHeight.has_value())
        return;

    ui->sliderRangeLower->setValue(0);
    ui->sliderRangeLower->setMinimum(0);
    ui->sliderRangeLower->setMaximum(*imageHeight - 1);
    ui->sliderRangeLower->setSingleStep(1);

    ui->sliderRangeUpper->setValue(*imageHeight - 1);
    ui->sliderRangeUpper->setMinimum(0);
    ui->sliderRangeUpper->setMaximum(*imageHeight - 1);
    ui->sliderRangeUpper->setSingleStep(1);

    ui->lineEditRangeLower->setText("0");
    ui->lineEditRangeLower->setValidator(new QIntValidator(0, *imageHeight - 1));

    ui->lineEditRangeUpper->setText(QString::number(*imageHeight - 1));
    ui->lineEditRangeUpper->setValidator(new QIntValidator(0, *imageHeight - 1));
}

Panel::~Panel()
{
    delete ui;
}

void Panel::connectSignals()
{
    connectSlidersConstraints();

    connect(ui->sliderRangeLower, &QSlider::valueChanged, [this](int value) { ui->lineEditRangeLower->setText(QString::number(value)); });
    connect(ui->sliderRangeUpper, &QSlider::valueChanged, [this](int value) { ui->lineEditRangeUpper->setText(QString::number(value)); });

    connect(ui->lineEditRangeLower, &QLineEdit::textChanged, [this](const QString& text) { ui->sliderRangeLower->setValue(text.toInt()); });
    connect(ui->lineEditRangeUpper, &QLineEdit::textChanged, [this](const QString& text) { ui->sliderRangeUpper->setValue(text.toInt()); });

    connect(ui->sliderGainValueLower, &QSlider::valueChanged, [this](int value) {
        ui->labelGainValueLower->setText(QString::number(value/multiplier));
        emit sliderGainChanged(
            ui->sliderRangeLower->value(),
            ui->sliderRangeUpper->value(),
            value/multiplier,
            ui->sliderGainValueUpper->value()/multiplier);
    });
    connect(ui->sliderGainValueUpper, &QSlider::valueChanged, [this](int value) {
        ui->labelGainValueUpper->setText(QString::number(value/multiplier));
        emit sliderGainChanged(
            ui->sliderRangeLower->value(),
            ui->sliderRangeUpper->value(),
            ui->sliderGainValueLower->value()/multiplier,
            value/multiplier);
    });

    connect(ui->buttonEqualizeHist, &QPushButton::clicked, [this] () {
        emit buttonEqualizeHistClicked(ui->sliderRangeLower->value(), ui->sliderRangeUpper->value());
    });

    connect(ui->buttonRotate, &QPushButton::clicked, [this] () { emit buttonRotateClicked(); });
    connect(ui->buttonReset, &QPushButton::clicked, [this] () { emit buttonResetClicked(); });
}

void Panel::connectSlidersConstraints()
{
    connect(ui->sliderRangeLower, &QSlider::sliderMoved, [this](int value) {
        if (auto upperSliderValue = ui->sliderRangeUpper->value(); value >= upperSliderValue)
            ui->sliderRangeLower->setValue(upperSliderValue - 1);
    });

    connect(ui->sliderRangeUpper, &QSlider::sliderMoved, [this](int value) {
       if (auto lowerSliderValue = ui->sliderRangeLower->value(); value <= lowerSliderValue)
           ui->sliderRangeUpper->setValue(lowerSliderValue + 1);
    });

    connect(ui->sliderGainValueLower, &QSlider::sliderMoved, [this](int value) {
        if (auto upperSliderValue = ui->sliderGainValueUpper->value(); value >= upperSliderValue)
            ui->sliderGainValueLower->setValue((upperSliderValue - 1));
    });

    connect(ui->sliderGainValueUpper, &QSlider::sliderMoved, [this](int value) {
        if (auto lowerSliderValue = ui->sliderGainValueLower->value(); value <= lowerSliderValue)
            ui->sliderGainValueUpper->setValue(lowerSliderValue + 1);
    });
}
