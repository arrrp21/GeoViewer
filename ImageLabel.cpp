#include "ImageLabel.hpp"
#include <QPainter>
#include <QDebug>

ImageLabel::ImageLabel(float& scaleFactor, QWidget* parent)
    : QLabel(parent)
    , scaleFactor{scaleFactor}
{
    setContentsMargins(leftAxisWidth, 0, 0, bottomAxisHeight);
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
    if (relativePosition.x() > leftAxisWidth and relativePosition.y() < height() - bottomAxisHeight)
    {
        emit mouseMoved(relativePosition.x() - leftAxisWidth - 1, relativePosition.y());
    }
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

void ImageLabel::paintEvent(QPaintEvent* event)
{
    qDebug() << "ImageLabel width: " << width() << " height: " << height();

    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 10));

    drawAxes(painter);
}

void ImageLabel::drawAxes(QPainter& painter)
{
    drawYAxis(painter);
    drawXAxis(painter);
    drawUnit(painter);
}

void ImageLabel::drawXAxis(QPainter& painter)
{
    painter.fillRect(leftAxisWidth, height() - bottomAxisHeight, width() - leftAxisWidth, bottomAxisHeight, QColorConstants::Gray);

    if (xStep.has_value())
    {
        double axisPixelsStep = xAxisStep / xStep.value() * scaleFactor;
        double xPos = axisPixelsStep + leftAxisWidth;
        int yBegin = height() - bottomAxisHeight;
        int yEnd = yBegin + bottomAxisHeight / 3;
        double axisValue = xAxisStep;
        while (xPos < width())
        {
            painter.drawLine(qRound(xPos), yBegin, qRound(xPos), yEnd);
            const QRect boundingRect(qRound(xPos) - qRound(axisPixelsStep)/2, yEnd, qRound(axisPixelsStep), height() - yEnd);
            painter.drawText(boundingRect, Qt::AlignCenter, QString::number(axisValue));

            xPos += axisPixelsStep;
            axisValue += xAxisStep;
        }
    }
}

void ImageLabel::drawYAxis(QPainter& painter)
{
    painter.fillRect(0, 0, leftAxisWidth, height(), QColorConstants::Gray);

    if (yStep.has_value())
    {
        double axisPixelsStep = yAxisStep / yStep.value() * scaleFactor;
        double yPos = axisPixelsStep;
        int xBegin = leftAxisWidth * 2 / 3;
        int xEnd = leftAxisWidth;
        double axisValue = yAxisStep;
        while (yPos < (height() - bottomAxisHeight))
        {
            painter.drawLine(xBegin, qRound(yPos), xEnd, qRound(yPos));
            const QRect boundingRect(0, qRound(yPos) - qRound(axisPixelsStep)/2, xEnd, qRound(axisPixelsStep));
            painter.drawText(boundingRect, Qt::AlignCenter, QString::number(axisValue));

            yPos += axisPixelsStep;
            axisValue += yAxisStep;
        }
    }
}

void ImageLabel::drawUnit(QPainter& painter)
{
    if (xStep.has_value() and yStep.has_value())
    {
        const QRect boundingRect(0, height() - bottomAxisHeight, leftAxisWidth, bottomAxisHeight);
        painter.drawText(boundingRect, Qt::AlignCenter, "m");
    }
}

void ImageLabel::setXStep(double xStep)
{
    this->xStep = xStep;
}

void ImageLabel::setYStep(double yStep)
{
    this->yStep = yStep;
}
