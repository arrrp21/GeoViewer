#pragma once

#include <QMainWindow>
#include <QImage>
#include <QDebug>
#include <QLabel>
#include <QScrollArea>

#include "ImageLabel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QImageWrapper;
class ImageTransformer;

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
    QScrollArea* scrollArea;

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

public slots:
    void on_actionOpenTriggered(bool);
    void on_actionGrayscaleTriggered(bool);
    void on_actionRotate_90Triggered(bool);
    void on_mouseWheelUsed(QPoint angleDelta);
    void on_mouseMoved(int x, int y);
};
