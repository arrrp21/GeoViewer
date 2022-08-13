#pragma once
#include <QLabel>
#include <QWheelEvent>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    ImageLabel(float& scaleFactor, QWidget* parent = nullptr);

public:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void paintEvent(QPaintEvent*) override;
    void setXStep(double xStep);
    void setYStep(double yStep);
    void showImage(const QImage&);
    void clearImage();
    static constexpr int leftAxisWidth = 40;
    static constexpr int bottomAxisHeight = 30;

private:
    void drawAxes(QPainter&);
    void drawXAxis(QPainter&);
    void drawYAxis(QPainter&);
    void drawUnit(QPainter&);
    QPoint currentCursorPosition;
    bool isMousePressed{false};
    float& scaleFactor;
    std::optional<double> xStep{std::nullopt};
    std::optional<double> yStep{std::nullopt};
    static constexpr double yAxisStep{0.5};
    static constexpr double xAxisStep{1.0};

signals:
    void mouseWheelUsed(QPoint angleDelta);
    void mousePressedMoved(int x, int y);
    void mouseMoved(int x, int y);
    void mousePressed();
};

