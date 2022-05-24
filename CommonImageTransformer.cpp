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

    const int newHeight = imageWrapper.width();
    const int newWidth = imageWrapper.height();

    qDebug() << "newWidth: " << newWidth << ", newHeight: " << newHeight;

    ImageData newData(newWidth, newHeight);
    qDebug() << "newWidth: " << newData.getWidth() << ", newHeight: " << newData.getHeight();
    GprData::DataType* castedNewData = newData.getData();

    for (int h = 0; h < newHeight; h++)
        for (int w = 0; w < newWidth; w++)
            castedNewData[h * newWidth + w] = castedOldData[(newWidth - w - 1) * newHeight + h];

    imageWrapper.changeOriginalImageData(newData);
    imageWrapper.setNewImage(std::move(newData));
}


void CommonImageTransformer::changeContrast(float contrast)
{
    qDebug() << "changeContrast: " << contrast;
    LookupTable lut = createLut(contrast);

    ImageData newImageData(imageWrapper.width(), imageWrapper.height());
    const auto size = newImageData.getSize();

    const ImageData oldImageData = imageWrapper.getOriginalImageData();
    for (int i = 0; i < size; i++)
    {
        newImageData[i] = lut[oldImageData[i]];
    }

    imageWrapper.setNewImage(std::move(newImageData));
}

void CommonImageTransformer::gain(int from, int to, float value)
{
    ImageData newImageData{imageWrapper.getOriginalImageData()};

    for (int i = from; i < to; i++)
    {
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            std::uint64_t tempNewValue = static_cast<std::uint64_t>(newImageData.at(i, j) * value);
            GprData::DataType newValue = tempNewValue <= limits::max() ? tempNewValue : limits::max();
            newImageData.at(i, j) = newValue;
        }
    }

    imageWrapper.setNewImage(std::move(newImageData));
}

void CommonImageTransformer::equalizeHistogram(int from, int to)
{
    qDebug() << "equalizeHistogram from: " << from << ", to: " << to;
    qDebug() << "min: " << min(from, to) << ", max: " << max(from, to);

    ImageData newImageData{imageWrapper.getOriginalImageData()};
    LookupTable lut = createLut(min(from, to), max(from, to));
    auto mi = min(from, to);
    auto ma = max(from, to);

    qDebug() << "lut created";

    for (int i = from; i < to; i++)
    {
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            newImageData.at(i, j) = lut[newImageData.at(i, j)];
            //newImageData.at(i, j) = std::lround((newImageData.at(i, j) - mi) / (ma - mi) * (limits::max() - 1));
        }
    }
    qDebug() << "before setNewImage";

    imageWrapper.setNewImage(std::move(newImageData));
    qDebug() << "after setNewImage";
    qDebug() << "min: " << min(from, to) << ", max: " << max(from, to);
    qDebug() << "====================================";
}

LookupTable CommonImageTransformer::createLut(GprData::DataType min, GprData::DataType max)
{
    LookupTable lut{};

    const double formula = (limits::max() - 1) / (max - min);
    for (auto i = min; i < max; i++)
    {
        lut[i] = std::lround(static_cast<double>(i - min) * formula);
        //lut[i] = std::lround((i - min) / (max - min) * (limits::max() - 1));
    }

    return lut;
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

GprData::DataType CommonImageTransformer::min(int from, int to)
{
    const ImageData& imageData = imageWrapper.getOriginalImageData();
    GprData::DataType min = imageData[0];

    for (auto i = from; i < to; i++)
        for (int j = 0; j < imageWrapper.width(); j++)
            if (imageData.at(i, j) < min)
                min = imageData.at(i, j);

    return min;
}

GprData::DataType CommonImageTransformer::max(int from, int to)
{
    const ImageData& imageData = imageWrapper.getOriginalImageData();
    GprData::DataType max = imageData[0];

    for (auto i = from; i < to; i++)
        for (auto j = 0; j < imageWrapper.width(); j++)
            if (imageData[i] > max)
                max = imageData.at(i, j);

    return max;
}
