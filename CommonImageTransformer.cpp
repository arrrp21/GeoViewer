#include "CommonImageTransformer.hpp"
#include "QImageWrapper.hpp"
#include "Mask.hpp"
#include "Log.hpp"

namespace image_transforming
{
CommonImageTransformer::CommonImageTransformer(QImageWrapper& imageWrapper)
 : imageWrapper{imageWrapper}
{
}

void CommonImageTransformer::rotate90()
{
    const ImageData& previousImageData = imageWrapper.getPreviousImageData();

    const int newHeight = imageWrapper.width();
    const int newWidth = imageWrapper.height();

    ImageData newData(newWidth, newHeight);

    for (int h = 0; h < newHeight; h++)
        for (int w = 0; w < newWidth; w++)
            newData.at(h, w) = previousImageData.at(newWidth - w - 1, h);

    imageWrapper.setNewImage(std::move(newData));
}

void CommonImageTransformer::gain(int from, int to, double gainLower, double gainUpper)
{
    timer.start("CPU linear gain");
    if (from < 0 or to >= imageWrapper.height())
        return;

    ImageData newImageData{imageWrapper.getPreviousImageData()};

    double step = (gainUpper - gainLower) / (to - from + 1);
    double gain = gainLower;
    for (int i = from; i <= to; i++)
    {
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            std::int32_t newValue = static_cast<std::int32_t>((static_cast<std::int32_t>(newImageData.at(i, j)) - limits::halfmax) * gain);
            newValue += limits::halfmax;
            if (newValue > limits::max)
            {
                newValue = limits::max;
            }
            else if (newValue < 0)
            {
                newValue = 0;
            }
            newImageData.at(i, j) = static_cast<GprData::DataType>(newValue);
        }
        gain += step;
    }
    timer.stop();

    imageWrapper.setNewImage(std::move(newImageData));
}

void CommonImageTransformer::equalizeHistogram(int from, int to)
{
    ImageTransformer::equalizeHistogram(imageWrapper, from, to);
}

void CommonImageTransformer::applyFilter(const Mask& mask)
{
    std::visit([this] (const auto& mask) { applyFilter(mask); }, mask);
}

void CommonImageTransformer::backgroundRemoval()
{
    timer.start("CPU background removal");
    int height = imageWrapper.height();
    int width = imageWrapper.width();

    const ImageData& imageData = imageWrapper.getPreviousImageData();
    ImageData newImageData{imageData};

    for (int i = 0; i < height; i++)
    {
        std::int32_t sum = 0;
        for (int j = 0; j < width; j++)
        {
            sum += (static_cast<std::int32_t>(imageData.at(i, j)) - limits::halfmax);
        }
        long mean = std::lround(static_cast<double>(sum)/width);

        for (int j = 0; j < width; j++)
        {
            long value = static_cast<long>(imageData.at(i, j)) - mean;
            if (value < 0)
                value = 0;
            else if (value > limits::max)
                value = limits::max;
            newImageData.at(i, j) = value;
        }
    }
    timer.stop();

    imageWrapper.setNewImage(std::move(newImageData));;
}

void CommonImageTransformer::trimTop()
{
    ImageTransformer::trimTop(imageWrapper);
}

template <class MaskType>
void CommonImageTransformer::applyFilter(const MaskType& mask)
{
    timer.start(fmt::format("CPU applyFilter int {}x{}", mask.height, mask.width));

    int width = imageWrapper.width();
    int height = imageWrapper.height();
    const ImageData& oldImageData = imageWrapper.getPreviousImageData();
    ImageData newImageData(width, height);

    int midHeight = mask.height/2;
    int midWidth = mask.width/2;

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
            if (value > limits::max)
                value = limits::max;
            else if (value < 0)
            {
                value = 0;
            }
            newImageData.at(row, col) = static_cast<GprData::DataType>(value);
        }
    }

    fillEdges(newImageData, midHeight, midWidth);

    timer.stop();

    imageWrapper.setNewImage(std::move(newImageData));
}
} // namespace image_transforming
