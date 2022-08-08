#include "Panel.hpp"
#include "ui_Panel.h"
#include "State.hpp"

#include <QIntValidator>
#include <QDoubleValidator>
#include <QSignalBlocker>

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

    ui->groupBoxGain->setEnabled(false);

    connectSignals();
}

void Panel::setImageHeight(int imageHeight)
{
    this->imageHeight = imageHeight;
    setHeightConstraints();
}

std::pair<int, int> Panel::getSliderRangeValues()
{
    return std::make_pair(ui->sliderRangeLower->value(), ui->sliderRangeUpper->value());
}

std::pair<double, double> Panel::getLinearGainValues()
{
    return std::make_pair(
        ui->sliderGainValueLower->value()/multiplier,
        ui->sliderGainValueUpper->value()/multiplier);
}

std::pair<int, int> Panel::getLinearGainSliderValues()
{
    return std::make_pair(ui->sliderGainValueLower->value(), ui->sliderGainValueUpper->value());
}

void Panel::restoreState(const State& state)
{
    QSignalBlocker blocker(this);
    ui->sliderRangeLower->setValue(state.sliderRangeLowerValue);
    ui->sliderRangeUpper->setValue(state.sliderRangeUpperValue);
    ui->sliderGainValueLower->setValue(state.sliderGainLowerValue);
    ui->sliderGainValueUpper->setValue(state.sliderGainUpperValue);
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

    connect(ui->rbEqualizeHist, &QRadioButton::toggled, [this] (bool checked) { if (checked) { emit rbEqualizeHistChecked(); } } );
    connect(ui->rbGain, &QRadioButton::toggled, [this] (bool checked) {
        if (checked)
        {
            ui->groupBoxGain->setEnabled(true);
            emit rbGainChecked();
        }
        else
        {
            ui->groupBoxGain->setEnabled(false);
        }
    });

    connect(ui->sliderRangeLower, &QSlider::valueChanged, [this] (int from) {
        emit sliderRangeChanged(from, ui->sliderRangeUpper->value());
    });
    connect(ui->sliderRangeUpper, &QSlider::valueChanged, [this] (int to) {
        emit sliderRangeChanged(ui->sliderRangeLower->value(), to);
    });

    connect(ui->buttonApply, &QPushButton::clicked, [this]() { emit buttonApplyClicked(); });
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
