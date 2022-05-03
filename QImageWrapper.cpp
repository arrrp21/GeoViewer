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

GprData::DataType QImageWrapper::getColor(int x, int y)
{
    const GprData::DataType* data = reinterpret_cast<GprData::DataType*>(rawData.getData());

    if (x >= image.width() or y >= image.height())
    {
        return 0u;
    }

    return data[y * image.width() + x];
}

const QImage& QImageWrapper::getImage() { return image; }

void QImageWrapper::setNewImage(QImage&& newImage, Span&& newData)
{
    qDebug() << "old image width: " << image.width() << " height: " << image.height();
    qDebug() << "old rawData size: " << rawData.getSize();

    image = std::move(newImage);
    rawData = std::move(newData);

    qDebug() << "new image width: " << image.width() << " height: " << image.height();
    qDebug() << "new rawData size: " << rawData.getSize();
}
