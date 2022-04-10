#include "QImageWrapper.hpp"
#include <utility>

QImageWrapper::QImageWrapper(GprData& gprData)
    : rawData{gprData.data}
{
    image = QImage(
        rawData.getData(),
        gprData.N_ACQ_SAMPLE,
        gprData.N_ACQ_SWEEP,
        gprData.N_ACQ_SAMPLE * 2,
        QImage::Format_Grayscale16);
}

const QImage& QImageWrapper::getImage() { return image; }

void QImageWrapper::setNewImage(QImage&& newImage, Span&& newData)
{
    image = std::move(newImage);
    rawData = std::move(newData);
}
