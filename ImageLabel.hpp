#pragma once
#include <QLabel>
#include <QWheelEvent>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    ImageLabel(QWidget* parent = nullptr);

public:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

private:
    QPoint currentCursorPosition;
    bool isMousePressed;

signals:
    void mouseWheelUsed(QPoint angleDelta);
    void mousePressedMoved(int x, int y);
    void mouseMoved(int x, int y);
    void mousePressed();
};

