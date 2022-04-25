#include "ImageLabel.hpp"


ImageLabel::ImageLabel(QWidget* parent)
    : QLabel(parent)
{
    setMouseTracking(true);
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint newPosition = event->globalPosition().toPoint();
    if (isMousePressed)
    {
        QPoint displacement = currentCursorPosition - newPosition;
        emit mousePressedMoved(displacement.x(), displacement.y());
    }
    currentCursorPosition = newPosition;
    QPoint relativePosition = event->pos();
    emit mouseMoved(relativePosition.x(), relativePosition.y());
}

void ImageLabel::mousePressEvent(QMouseEvent *event)
{
    currentCursorPosition = event->globalPosition().toPoint();
    isMousePressed = true;
    setCursor(Qt::ClosedHandCursor);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent*)
{
    isMousePressed = false;
    setCursor(Qt::ArrowCursor);
}

void ImageLabel::wheelEvent(QWheelEvent* event)
{
    emit mouseWheelUsed(event->angleDelta());
}
