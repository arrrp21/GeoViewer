#include "ImageLabel.hpp"


void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint displacement = currentCursorPosition - event->globalPosition().toPoint();
    currentCursorPosition = event->globalPosition().toPoint();
    emit mouseMoved(displacement.x(), displacement.y());
}

void ImageLabel::mousePressEvent([[maybe_unused]] QMouseEvent *event)
{
    currentCursorPosition = event->globalPosition().toPoint();
    setCursor(Qt::ClosedHandCursor);
}

void ImageLabel::mouseReleaseEvent([[maybe_unused]] QMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
}

void ImageLabel::wheelEvent(QWheelEvent* event)
{
    emit mouseWheelUsed(event->angleDelta());
}
