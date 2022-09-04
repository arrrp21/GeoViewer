#pragma once
#include <QWidget>

struct State;

QT_BEGIN_NAMESPACE
class QRadioButton;
QT_END_NAMESPACE

namespace Ui {
class Panel;
}

class Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = nullptr);
    void setImageHeight(int imageHeight);
    void resetGainSliders();
    std::pair<int, int> getSliderRangeValues();
    std::pair<double, double> getLinearGainValues();
    std::pair<int, int> getLinearGainSliderValues();
    void restoreState(const State&);
    void uncheckRadioButton();
    QRadioButton* findCheckedRadioButton();
    ~Panel();


private:
    static constexpr double minGain{-120.0};
    static constexpr double maxGain{300.0};
    static constexpr double defaultGainLower{1.0};
    static constexpr double defaultGainUpper{10.0};
    static constexpr double multiplier{1.0};

    Ui::Panel *ui;
    std::optional<int> imageHeight{std::nullopt};

    void setHeightConstraints();
    void connectSignals();
    void connectSlidersConstraints();

signals:
    void sliderGainChanged(int, int, double, double);
    void sliderRangeChanged(int, int);
    void buttonRotateClicked();
    void buttonResetClicked();
    void rbEqualizeHistChecked();
    void rbGainChecked();
    void buttonApplyClicked();
    void buttonCancelClicked();
};
