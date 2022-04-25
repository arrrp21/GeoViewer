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
    GprData::DataType* castedNewData = reinterpret_cast<GprData::DataType*>(newData.getData());

    for (int h = 0; h < newHeight; h++)
        for (int w = 0; w < newWidth; w++)
            castedNewData[h * newWidth + w] = castedOldData[(newWidth - w - 1) * newHeight + h];

    imageWrapper.setNewImage(
        QImage(newData.getData(), newWidth, newHeight, newWidth * sizeof(GprData::DataType), QImage::Format_Grayscale16),
        std::move(newData));
}
