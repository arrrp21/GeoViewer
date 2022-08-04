#include "CommonImageTransformer.hpp"
#include "QImageWrapper.hpp"
#include "Mask.hpp"

namespace image_transforming
{
namespace
{
GprData::DataType meanInRow(const GprData::DataType* values, int size)
{
    double sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += static_cast<double>(values[i]);
    }

    return static_cast<GprData::DataType>(sum/size);
}

GprData::DataType findMax(const GprData::DataType* values, int size)
{
    GprData::DataType maxValue = 0;
    for (int i = 0; i < size; i++)
    {
        if (values[i] > maxValue)
            maxValue = values[i];
    }
    return maxValue;
}
} // namespace

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
    if (from < 0 or to >= imageWrapper.height())
        return;

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
    const ImageData& imageData = imageWrapper.getOriginalImageData();

    qDebug() << "equalizeHistogram from: " << from << ", to: " << to;
    qDebug() << "min: " << min(from, to, imageData) << ", max: " << max(from, to, imageData);

    ImageData newImageData{imageWrapper.getOriginalImageData()};
    LookupTable lut = createLut(min(from, to, imageData), max(from, to, imageData));

    qDebug() << "lut created";

    for (int i = from; i < to; i++)
    {
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            newImageData.at(i, j) = lut[newImageData.at(i, j)];
        }
    }
    qDebug() << "before setNewImage";

    qDebug() << "after setNewImage";
    qDebug() << "min: " << min(from, to, newImageData) << ", max: " << max(from, to, newImageData);
    qDebug() << "====================================";
    imageWrapper.setNewImage(std::move(newImageData));
}

void CommonImageTransformer::applyFilter(const Mask& mask)
{
    std::visit([this] (const auto& mask) {applyFilter(mask); }, mask);
}

void CommonImageTransformer::backgroundRemoval()
{
    int height = imageWrapper.height();
    int width = imageWrapper.width();

    const ImageData& imageData = imageWrapper.getOriginalImageData();
    ImageData newImageData{imageData};

    for (int i = 0; i < height; i++)
    {
        std::uint32_t sum = 0;
        for (int j = 0; j < width; j++)
        {
            sum += imageData.at(i, j);
        }
        GprData::DataType mean = static_cast<GprData::DataType>(std::lround(static_cast<double>(sum)/width));

        for (int j = 0; j < width; j++)
        {
            GprData::DataType value = imageData.at(i, j) <= mean ? 0 : imageData.at(i, j) - mean;
            newImageData.at(i, j) = value;
        }
    }

    imageWrapper.setNewImage(std::move(newImageData));
}

void CommonImageTransformer::trimTop()
{
    const ImageData& imageData = imageWrapper.getImageData();

    GprData::DataType minMean = limits::max();
    int rowWithMinMean = 0;
    for (int row = 0; row < imageWrapper.height(); row++)
    {
        GprData::DataType mean = meanInRow(imageData.getDataAt(row, 0), imageData.getWidth());
        if (mean < minMean)
        {
            minMean = mean;
            rowWithMinMean = row;
        }
    }
    qDebug() << "rowWithMaxValue: " << rowWithMinMean;

    ImageData newImageData{imageData};

    newImageData.trimTop(rowWithMinMean);
    imageWrapper.setNewImage(std::move(newImageData));
    qDebug() << "trimTop width: " << imageWrapper.width() << " height: " << imageWrapper.height();
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

    if (max - min <= 0)
    {
        return lut;
    }

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

GprData::DataType CommonImageTransformer::min(int from, int to, const ImageData& imageData)
{
    GprData::DataType min = imageData[0];

    for (auto i = from; i < to; i++)
        for (int j = 0; j < imageWrapper.width(); j++)
            if (imageData.at(i, j) < min)
                min = imageData.at(i, j);

    return min;
}

GprData::DataType CommonImageTransformer::max(int from, int to, const ImageData& imageData)
{
    GprData::DataType max = imageData[0];

    for (auto i = from; i < to; i++)
        for (auto j = 0; j < imageWrapper.width(); j++)
            if (imageData[i] > max)
                max = imageData.at(i, j);

    return max;
}
} // namespace image_transforming
