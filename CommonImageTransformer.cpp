#include "CommonImageTransformer.hpp"
#include "QImageWrapper.hpp"
#include <QVariant>

CommonImageTransformer::CommonImageTransformer(QImageWrapper& imageWrapper)
 : imageWrapper{imageWrapper}
{
}

void CommonImageTransformer::rotate90()
{
    const uchar* oldData = imageWrapper.getImage().constBits();
    const GprData::DataType* castedOldData = reinterpret_cast<const GprData::DataType*>(oldData);

    const int newHeight = imageWrapper.getImage().width();
    const int newWidth = imageWrapper.getImage().height();

    Span newData(newWidth * newHeight * sizeof(GprData::DataType));
    GprData::DataType* castedNewData = newData.getData();

    for (int h = 0; h < newHeight; h++)
        for (int w = 0; w < newWidth; w++)
            castedNewData[h * newWidth + w] = castedOldData[(newWidth - w - 1) * newHeight + h];

    imageWrapper.setNewImage(
        QImage(newData.getByteData(), newWidth, newHeight, newWidth * sizeof(GprData::DataType), QImage::Format_Grayscale16),
        std::move(newData));
}


void CommonImageTransformer::changeContrast(float contrast)
{
    qDebug() << "changeContrast";
    LookupTable lut = createLut(contrast);

    const auto sizeInBytes = imageWrapper.getRawData().getSizeInBytes();
    Span newData(sizeInBytes);
    GprData::DataType* castedNewData = newData.getData();
    const auto size = newData.getSize();

    const GprData::DataType* castedOldData = imageWrapper.getOriginalRawData().getData();
    for (auto i = 0u; i < size; i++)
    {
        castedNewData[i] = lut[castedOldData[i]];
    }

    const QImage& image = imageWrapper.getImage();
    imageWrapper.setNewImage(
        QImage(newData.getByteData(), image.width(), image.height(), image.width() * sizeof(GprData::DataType), QImage::Format_Grayscale16),
        std::move(newData));
}

LookupTable CommonImageTransformer::createLut(float contrast)
{
    LookupTable lut;

    constexpr auto max = limits::max();
    constexpr auto max2 = max/2;
    for (auto i = 0u; i < lut.size(); i++)
    {
        const auto value = contrast * (i - max2) + max2;
        if (value < 0)
            lut[i] = 0;
        else if (0 <= value and value <= max)
            lut[i] = contrast * (i - max2) + max2;
        else
            lut[i] = max;
    }

    return lut;
}
