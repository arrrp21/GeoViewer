#pragma once

#include <QDialog>
#include <QLineEdit>
#include <array>
#include "Mask.hpp"
#include "Log.hpp"

namespace Ui {
class ChooseMaskDialog;
}

namespace image_transforming
{
using Mask = std::variant<details::Mask<int>, details::Mask<double>>;
struct PredefinedMask;
} // namespace image_transforming

class ChooseMaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseMaskDialog(QWidget *parent = nullptr);
    image_transforming::Mask getMask();
    ~ChooseMaskDialog();

private:
    static constexpr int horizontalInterspace{10};
    static constexpr int verticalInterspace{10};
    static constexpr int lineEditWidth{30};
    static constexpr int lineEditHeight{20};
    static constexpr int firstLineEditX{20};
    static constexpr int firstLineEditY{20};

    static constexpr int defaultMaskSize{3};
    const QString customFilter{"Custom filter"};

    static constexpr std::size_t maxMaskSize{5};
    Ui::ChooseMaskDialog *ui;
    std::array<std::array<QLineEdit*, maxMaskSize>, maxMaskSize> lineEdits;

    void drawMaskInputs(int);
    void fillLineEdits(const image_transforming::PredefinedMask&, int maskSize);

    template <typename MaskType>
    void fillLineEdits(const MaskType& mask)
    {
        for (int i = 0; i < mask.height; i++)
            for (int j = 0; j < mask.width; j++)
            {
                LOG_INFO("mask.at({}, {}) = {}", i, j, mask.at(i, j));
                lineEdits[i][j]->setText(QString::number(mask.at(i, j)));
                LOG_INFO("lineEdits[{}][{}] = {}", i, j, lineEdits[i][j]->text().toStdString());
            }
    }

    void clearLineEdits();
    bool validateMask();
    void connectSignals();
    std::unique_ptr<image_transforming::Mask> mask;

private slots:
    void on_spinBoxMaskSizeChanged(int);
    void on_comboBoxPredefinedMaskTextActivated(const QString&);
    void on_buttonOkClicked(bool);
    void on_buttonCancelClicked(bool);
    void on_lineEditTextEdited(const QString&);
};

