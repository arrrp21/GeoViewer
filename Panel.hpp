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
    ~Panel();


private:
    static constexpr double minGain{0.5};
    static constexpr double maxGain{2.0};
    static constexpr double defaultGain{1.0};

    Ui::Panel *ui;
    std::optional<int> imageHeight{std::nullopt};

    void setHeightConstraints();
    void connectSignals();
    void connectSlidersConstraints();

signals:
    void sliderGainValueChanged(int, int, int);
    void buttonEqualizeHistClicked(int, int);
};
