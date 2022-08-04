#pragma once
#include <QWidget>

namespace Ui {
class Panel;
}

class Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = nullptr);
    void setImageHeight(int imageHeight);
    std::pair<double, double> getLinearGainValues();
    ~Panel();


private:
    static constexpr double minGain{0.5};
    static constexpr double maxGain{2.0};
    static constexpr double defaultGainLower{1.0};
    static constexpr double defaultGainUpper{1.5};
    static constexpr double multiplier{100.0};

    Ui::Panel *ui;
    std::optional<int> imageHeight{std::nullopt};

    void setHeightConstraints();
    void connectSignals();
    void connectSlidersConstraints();

signals:
    void sliderGainChanged(int, int, double, double);
    void sliderRangeChanged(int, int);
    void buttonEqualizeHistClicked(int, int);
    void buttonRotateClicked();
    void buttonResetClicked();
    void rbEqualizeHistChecked();
    void rbGainChecked();
};
