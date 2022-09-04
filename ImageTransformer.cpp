#include "ImageTransformer.hpp"
#include "ImageData.hpp"
#include "QImageWrapper.hpp"
#include "Log.hpp"

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
} // namespace

void ImageTransformer::equalizeHistogram(QImageWrapper& imageWrapper, int from, int to)
{
    const ImageData& imageData = imageWrapper.getPreviousImageData();

    ImageData newImageData{imageWrapper.getPreviousImageData()};

    auto minval = min(from, to, imageData);
    auto maxval = max(from, to, imageData);

    LookupTable lut = createLut(minval, maxval);

    for (int i = from; i < to; i++)
    {
        for (int j = 0; j < imageWrapper.width(); j++)
        {
            newImageData.at(i, j) = lut[imageData.at(i, j)];
        }
    }
    imageWrapper.setNewImage(std::move(newImageData));
}

void ImageTransformer::trimTop(QImageWrapper& imageWrapper)
{
    const ImageData& imageData = imageWrapper.getPreviousImageData();

    GprData::DataType minMean = limits::max;
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

    ImageData newImageData{imageData};

    newImageData.trimTop(rowWithMinMean + 2);
    imageWrapper.setNewImage(std::move(newImageData));
}

LookupTable ImageTransformer::createLut(GprData::DataType min, GprData::DataType max)
{
    LookupTable lut{};

    if (min == 0 and max == limits::max)
    {
        for (auto i = 0u; i < lut.size(); i++)
            lut[i] = i;
        return lut;
    }

    if (max - min <= 0)
    {
        return lut;
    }

    const double formula = static_cast<double>(limits::max - 1) / (max - min);
    for (auto i = min; i < max; i++)
    {
        lut[i] = std::lround(static_cast<double>(i - min) * formula);
    }

    return lut;
}

GprData::DataType ImageTransformer::min(int from, int to, const ImageData& imageData)
{
    GprData::DataType min = imageData.at(from, 0);

    int minrow = from;
    int mincol = 0;
    for (auto i = from; i < to; i++)
    {
        for (int j = 0; j < imageData.getWidth(); j++)
        {
            if (imageData.at(i, j) < min)
            {
                min = imageData.at(i, j);
                minrow = i;
                mincol = j;
            }
        }
    }

    LOG_INFO("minrow: {}, mincol: {}, min: {}, val: {}", minrow, mincol, min, imageData.at(minrow, mincol));
    return min;
}

GprData::DataType ImageTransformer::max(int from, int to, const ImageData& imageData)
{
    GprData::DataType max = imageData.at(from, 0);

    int maxrow = from;
    int maxcol = 0;
    for (auto i = from; i < to; i++)
    {
        for (auto j = 0; j < imageData.getWidth(); j++)
        {
            if (imageData.at(i, j) > max)
            {
                max = imageData.at(i, j);
                maxrow = i;
                maxcol = j;
            }
        }
    }
    LOG_INFO("maxrow: {}, maxcol: {}, max: {}, val: {}", maxrow, maxcol, max, imageData.at(maxrow, maxcol));

    return max;
}

void ImageTransformer::fillEdges(ImageData& imageData, int midHeight, int midWidth)
{
    fillUpperEdge(imageData, midHeight);
    fillRightEdge(imageData, midHeight, midWidth);
    fillLowerEdge(imageData, midHeight);
    fillLeftEdge(imageData, midHeight, midWidth);
}

void ImageTransformer::fillUpperEdge(ImageData& imageData, int midHeight)
{
    int width = imageData.getWidth();
    for (int row = 0; row < midHeight; row++)
    {
        for (int col = 0; col < width; col++)
        {
            imageData.at(row, col) = imageData.at(midHeight, col);
        }
    }
}

void ImageTransformer::fillRightEdge(ImageData& imageData, int midHeight, int midWidth)
{
    int height = imageData.getHeight();
    for (int row = midHeight; row < height - midHeight; row++)
    {
        for (int col = 0; col < midWidth; col++)
        {
            imageData.at(row, col) = imageData.at(row, midWidth);
        }
    }
}

void ImageTransformer::fillLowerEdge(ImageData& imageData, int midHeight)
{
    int height = imageData.getHeight();
    int width = imageData.getWidth();
    for (int row = height - midHeight; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            imageData.at(row, col) = imageData.at(height - midHeight - 1, col);
        }
    }
}

void ImageTransformer::fillLeftEdge(ImageData& imageData, int midHeight, int midWidth)
{
    int height = imageData.getHeight();
    int width = imageData.getWidth();
    for (int row = midHeight; row < height - midHeight; row++)
    {
        for (int col = width - midWidth; col < width; col++)
        {
            imageData.at(row, col) = imageData.at(row, width - midWidth - 1);
        }
    }
}
}
