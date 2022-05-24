#include "QImageWrapper.hpp"
#include <utility>

QImageWrapper::QImageWrapper(GprData& gprData)
    : imageData{gprData.data, gprData.N_ACQ_SAMPLE, gprData.N_ACQ_SWEEP}
    , originalImageData{imageData}
{
    image = QImage(
        imageData.getByteData(),
        gprData.N_ACQ_SAMPLE,
        gprData.N_ACQ_SWEEP,
        gprData.N_ACQ_SAMPLE * sizeof(GprData::DataType),
        QImage::Format_Grayscale16);
}

GprData::DataType QImageWrapper::getColor(int x, int y)
{
    if (x >= width() or y >= height())
    {
        return 0u;
    }

    return imageData.at(y, x);
}

int QImageWrapper::height() const { return image.height(); }
int QImageWrapper::width() const { return image.width(); }

const QImage& QImageWrapper::getImage() const { return image; }
const ImageData& QImageWrapper::getImageData() const { return imageData; }
const ImageData& QImageWrapper::getOriginalImageData() const { return originalImageData; }

void QImageWrapper::changeOriginalImageData(const ImageData& newOriginalImageData)
{
    originalImageData = newOriginalImageData;
}

void QImageWrapper::setNewImage(ImageData&& newImageData)
{
    imageData = std::move(newImageData);
    image = QImage(
        imageData.getByteData(),
        imageData.getWidth(),
        imageData.getHeight(),
        imageData.getWidth() * sizeof(GprData::DataType),
        QImage::Format_Grayscale16);
}

void QImageWrapper::resetImage()
{
    imageData = originalImageData;
    image = QImage(
        imageData.getByteData(),
        imageData.getWidth(),
        imageData.getHeight(),
        imageData.getWidth() * sizeof(GprData::DataType),
        QImage::Format_Grayscale16);
}
