#include <QLineEdit>
#include <QMessageBox>

#include "ChooseMaskDialog.hpp"
#include "ui_ChooseMaskDialog.h"
#include "PredefinedMasks.hpp"
#include "Log.hpp"

ChooseMaskDialog::ChooseMaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseMaskDialog)
{
    ui->setupUi(this);
    mask = std::make_unique<image_transforming::Mask>(image_transforming::details::Mask<int>(image_transforming::highPassFilter3x3));

    for (std::size_t i = 0; i < maxMaskSize; i++)
    {
        for (std::size_t j = 0; j < maxMaskSize; j++)
        {
            lineEdits[i][j] = new QLineEdit(this);
            lineEdits[i][j]->setValidator(new QDoubleValidator);
            lineEdits[i][j]->setGeometry(firstLineEditX + (lineEditWidth + horizontalInterspace) * i,
                                         firstLineEditY + (lineEditHeight + verticalInterspace) * j,
                                         lineEditWidth,
                                         lineEditHeight);
            lineEdits[i][j]->hide();
        }
    }

    ui->comboBoxPredefinedMask->addItem(image_transforming::highPassFilter.name);
    ui->comboBoxPredefinedMask->addItem(customFilter);

    connectSignals();

    drawMaskInputs(defaultMaskSize);
    fillLineEdits(image_transforming::highPassFilter, defaultMaskSize);
    ui->spinBoxMaskSize->setValue(defaultMaskSize);
}

image_transforming::Mask ChooseMaskDialog::getMask()
{
    if (mask)
    {
        return *mask;
    }
    return image_transforming::highPassFilter3x3;
}

ChooseMaskDialog::~ChooseMaskDialog()
{
    delete ui;
}

void ChooseMaskDialog::drawMaskInputs(int maskSize)
{
    for (std::size_t i = 0; i < maxMaskSize; i++)
        for (std::size_t j = 0; j < maxMaskSize; j++)
            lineEdits[i][j]->hide();

    for (int i = 0; i < maskSize; i++)
        for (int j = 0; j < maskSize; j++)
            lineEdits[i][j]->show();
}

void ChooseMaskDialog::fillLineEdits(const image_transforming::PredefinedMask& mask, int maskSize)
{
    switch (maskSize)
    {
    case 3:
        std::visit([this] (const auto& concreteMask) { fillLineEdits(concreteMask); }, mask.mask3x3);
        return;
    case 5:
        std::visit([this] (const auto& concreteMask) { fillLineEdits(concreteMask); }, mask.mask5x5);
        return;
    default:
        return;
    }
}

void ChooseMaskDialog::clearLineEdits()
{
    int maskSize = ui->spinBoxMaskSize->value();
    for (int i = 0; i < maskSize; i++)
        for (int j = 0; j < maskSize; j++)
            lineEdits[i][j]->clear();
}

bool ChooseMaskDialog::validateMask()
{
    bool isAtLeastOneDouble = false;
    int maskSize = ui->spinBoxMaskSize->value();
    std::vector<double> values{};
    values.reserve(maskSize * maskSize);
    bool isOk;
    for (int i = 0; i < maskSize; i++)
    {
        for (int j = 0; j < maskSize; j++)
        {
            auto lineEditText = lineEdits[i][j]->text();
            if (not isAtLeastOneDouble)
            {
                if (lineEditText.contains(',') or lineEditText.contains('.'))
                {
                    isAtLeastOneDouble = true;
                }
            }

            values.push_back(lineEditText.replace(",", ".").toDouble(&isOk));
            if (not isOk)
            {
                mask.reset();
                return false;
            }
        }
    }

    if (isAtLeastOneDouble)
    {
        mask = std::make_unique<image_transforming::Mask>(image_transforming::details::Mask<double>(maskSize));
        auto& maskDouble = std::get<image_transforming::details::Mask<double>>(*mask);
        for (const auto& val : values)
            maskDouble.add(val);
    }
    else
    {
        mask = std::make_unique<image_transforming::Mask>(image_transforming::details::Mask<int>(maskSize));
        auto& maskInt = std::get<image_transforming::details::Mask<int>>(*mask);
        for (const auto& val : values)
            maskInt.add(std::lround(val));
    }
    return true;
}

void ChooseMaskDialog::connectSignals()
{
    connect(ui->spinBoxMaskSize, SIGNAL(valueChanged(int)), this, SLOT(on_spinBoxMaskSizeChanged(int)));
    connect(
        ui->comboBoxPredefinedMask,
        SIGNAL(textActivated(QString)),
        this,
        SLOT(on_comboBoxPredefinedMaskTextActivated(QString)));
    connect(ui->buttonOk, SIGNAL(clicked(bool)), this, SLOT(on_buttonOkClicked(bool)));
    connect(ui->buttonCancel, SIGNAL(clicked(bool)), this, SLOT(on_buttonCancelClicked(bool)));

    for (std::size_t i = 0; i < maxMaskSize; i++)
    {
        for (std::size_t j = 0; j < maxMaskSize; j++)
        {
            connect(lineEdits[i][i], SIGNAL(textEdited(QString)), this, SLOT(on_lineEditTextEdited(QString)));
        }
    }
}

void ChooseMaskDialog::on_spinBoxMaskSizeChanged(int maskSize)
{
    drawMaskInputs(maskSize);
    if (ui->comboBoxPredefinedMask->currentText() == image_transforming::highPassFilter.name)
    {
        fillLineEdits(image_transforming::highPassFilter, maskSize);
    }
}

void ChooseMaskDialog::on_comboBoxPredefinedMaskTextActivated(const QString& text)
{
    if (text == image_transforming::highPassFilter.name)
    {
        fillLineEdits(image_transforming::highPassFilter, ui->spinBoxMaskSize->value());
    }
    else if (text == customFilter)
    {
        clearLineEdits();
    }
}

void ChooseMaskDialog::on_buttonOkClicked(bool)
{
    if (validateMask())
    {
        accept();
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Icon::Warning, "Wrong mask", "Mask has at least one incorrect value.");
        msgBox.exec();
    }
}

void ChooseMaskDialog::on_buttonCancelClicked(bool)
{
    reject();
}

void ChooseMaskDialog::on_lineEditTextEdited(const QString&)
{
    ui->comboBoxPredefinedMask->setCurrentText(customFilter);
}
