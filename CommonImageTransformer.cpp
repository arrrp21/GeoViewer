#include "CommonImageTransformer.hpp"
#include "QImageWrapper.hpp"
#include "Mask.hpp"
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

void CommonImageTransformer::gain(int from, int to, double gainLower, double gainUpper)
{
    ImageData newImageData{imageWrapper.getOriginalImageData()};

    double step = (gainUpper - gainLower) / (to - from);
    double gain = gainLower;
    for (int i = from; i < to; i++)
    {
        gain += step;
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            std::uint64_t tempNewValue = static_cast<std::uint64_t>(newImageData.at(i, j) * gain);
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

    ImageData newImageData{imageWrapper.getImageData()};
    LookupTable lut = createLut(min(from, to), max(from, to));

    qDebug() << "lut created";

    for (int i = from; i < to; i++)
    {
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            newImageData.at(i, j) = lut[newImageData.at(i, j)];
        }
    }
    qDebug() << "before setNewImage";

    imageWrapper.setNewImage(std::move(newImageData));
    qDebug() << "after setNewImage";
    qDebug() << "min: " << min(from, to) << ", max: " << max(from, to);
    qDebug() << "====================================";
}

void CommonImageTransformer::applyFilter(const Mask& mask)
{
    std::visit([this] (auto&& mask) {applyFilter(mask); }, mask);
}

template <class MaskType>
void CommonImageTransformer::applyFilter(const MaskType& mask)
{
    qDebug() << "applyFilter";

    int width = imageWrapper.width();
    int height = imageWrapper.height();
    const ImageData& oldImageData = imageWrapper.getImageData();
    ImageData newImageData(width, height);

    int midHeight = mask.height/2;
    int midWidth = mask.width/2;

    int c = 0;
    for (int row = midHeight; row < height - midHeight; row++)
    {
        for (int col = midWidth; col < width - midWidth; col++)
        {
            int value = 0;
            for (int i = 0; i < mask.height; i++)
            {
                for (int j = 0; j < mask.width; j++)
                    value += oldImageData.at(row + i - midHeight, col + j - midWidth) * mask.at(i, j);
            }
            if (value > limits::max())
                value = limits::max();
            else if (value < 0)
            {
                if (c++ % 100 == 0)
                    qDebug() << value;
                value = 0;
            }
            newImageData.at(row, col) = static_cast<GprData::DataType>(value);
        }
    }

    imageWrapper.setNewImage(std::move(newImageData));
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
