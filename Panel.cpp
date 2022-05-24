#include "Panel.hpp"
#include "ui_Panel.h"

#include <QIntValidator>
#include <QDoubleValidator>

Panel::Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Panel)
{
    ui->setupUi(this);

    ui->sliderGainValue->setMinimum(minGain * 10);
    ui->sliderGainValue->setMaximum(maxGain * 10);
    ui->sliderGainValue->setValue(defaultGain * 10);
    QDoubleValidator* validator = new QDoubleValidator(minGain, maxGain, 1);
    validator->setNotation(QDoubleValidator::Notation::StandardNotation);
    validator->setLocale(QLocale(QLocale::English));
    ui->lineEditGainValue->setValidator(validator);

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

    ui->sliderLower->setValue(0);
    ui->sliderLower->setMinimum(0);
    ui->sliderLower->setMaximum(*imageHeight - 1);
    ui->sliderLower->setSingleStep(1);

    ui->sliderUpper->setValue(*imageHeight - 1);
    ui->sliderUpper->setMinimum(0);
    ui->sliderUpper->setMaximum(*imageHeight - 1);
    ui->sliderUpper->setSingleStep(1);

    ui->lineEditLower->setText("0");
    ui->lineEditLower->setValidator(new QIntValidator(0, *imageHeight - 1));

    ui->lineEditUpper->setText(QString::number(*imageHeight - 1));
    ui->lineEditUpper->setValidator(new QIntValidator(0, *imageHeight - 1));
}

Panel::~Panel()
{
    delete ui;
}

void Panel::connectSignals()
{
    connectSlidersConstraints();

    connect(ui->sliderLower, &QSlider::valueChanged, [this](int value) { ui->lineEditLower->setText(QString::number(value)); });
    connect(ui->sliderUpper, &QSlider::valueChanged, [this](int value) { ui->lineEditUpper->setText(QString::number(value)); });

    connect(ui->lineEditLower, &QLineEdit::textChanged, [this](const QString& text) { ui->sliderLower->setValue(text.toInt()); });
    connect(ui->lineEditUpper, &QLineEdit::textChanged, [this](const QString& text) { ui->sliderUpper->setValue(text.toInt()); });

    connect(ui->sliderGainValue, &QSlider::valueChanged, [this](int value) {
        ui->lineEditGainValue->setText(QString::number(static_cast<float>(value)/10.0f));
        emit sliderGainValueChanged(ui->sliderLower->value(), ui->sliderUpper->value(), value);
    });
    connect(ui->lineEditGainValue, &QLineEdit::textChanged, [this](const QString& text) { ui->sliderGainValue->setValue(text.toFloat()*10); });

    connect(ui->buttonEqualizeHist, &QPushButton::clicked, [this] () {
        emit buttonEqualizeHistClicked(ui->sliderLower->value(), ui->sliderUpper->value());
    });
}

void Panel::connectSlidersConstraints()
{
    connect(ui->sliderLower, &QSlider::sliderMoved, [this](int value) {
        if (auto upperSliderValue = ui->sliderUpper->value(); value >= upperSliderValue)
            ui->sliderLower->setValue(upperSliderValue - 1);
    });

    connect(ui->sliderUpper, &QSlider::sliderMoved, [this](int value) {
       if (auto lowerSliderValue = ui->sliderLower->value(); value <= lowerSliderValue)
           ui->sliderUpper->setValue(lowerSliderValue + 1);
    });
}
