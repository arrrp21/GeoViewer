#pragma once

#include <QMainWindow>
#include <QImage>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QScrollArea;
class QLabel;
class QVBoxLayout;
class QScrollBar;
QT_END_NAMESPACE

class QImageWrapper;
class ImageTransformer;
class ImageLabel;
class Panel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<ImageTransformer> imageTransformer;
    std::unique_ptr<QImageWrapper> imageWrapper;
    QImage image;
    ImageLabel* imageLabel;
    Panel* panel;
    QScrollArea* scrollArea;
    QVBoxLayout* layout;
    QWidget* centralWidget;

    float scaleFactor{1.0f};

    static constexpr float maxScaleFactor{5.0f};
    static constexpr float minScaleFactor{0.25f};
    static constexpr float scaleFactorStep{0.1f};

    void print_image(uchar* buf)
    {
        for (int i = 0; i < 20; i++)
            qDebug() << buf[i];
        qDebug() << "=============================";
    }

    void scaleImage(float factor);
    void adjustScrollBar(QScrollBar* scrollBar, float factor);
    void connectSignals();

public slots:
    void on_actionGainPannelToggled(bool);
    void on_actionOpenTriggered(bool);
    void on_actionSaveTriggered(bool);
    void on_actionGrayscaleTriggered(bool);
    void on_actionRotate90Triggered(bool);
    void on_actionHighPassFilterTriggered(bool);
    void on_actionGpuAccelerationToggled(bool);
    void on_mouseWheelUsed(QPoint angleDelta);
    void on_mousePressedMoved(int x, int y);
    void on_mouseMoved(int x, int y);

    void on_sliderGainChanged(int, int, double, double);
    void on_buttonEqualizeHistClicked(int, int);
    void on_buttonResetClicked();
    void on_buttonRotateClicked();
};
