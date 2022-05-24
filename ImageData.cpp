#include "ImageData.hpp"

ImageData::ImageData(std::vector<GprData::DataType>& dataVector, int width, int height)
    : size{static_cast<int>(dataVector.size())}
    , sizeInBytes{static_cast<int>(dataVector.size() * sizeof(GprData::DataType))}
    , width{width}
    , height{height}
{
    data = new GprData::DataType[size];
    std::memcpy(data, dataVector.data(), sizeInBytes);
}

ImageData::~ImageData()
{
    delete [] data;
    sizeInBytes = 0u;
}

ImageData::ImageData(int width, int height)
    : size{width * height}
    , sizeInBytes(size * sizeof(GprData::DataType))
    , width{width}
    , height{height}
{
    data = new GprData::DataType[size];
}

ImageData::ImageData(const ImageData& other)
    : size{other.size}
    , sizeInBytes{other.sizeInBytes}
    , width{other.width}
    , height{other.height}
{
    data = new GprData::DataType[size];
    std::memcpy(data, other.getData(), sizeInBytes);
}

ImageData::ImageData(ImageData&& other) noexcept
    : data{other.data}
    , size{other.size}
    , sizeInBytes(other.sizeInBytes)
    , width{other.width}
    , height{other.height}
{
    other.data = nullptr;
    other.size = 0u;
    other.sizeInBytes = 0u;
    other.width = 0;
    other.height = 0;
}

ImageData& ImageData::operator=(const ImageData& other)
{
    if (data != nullptr)
        delete [] data;

    size = other.size;
    sizeInBytes = other.sizeInBytes;
    data = new GprData::DataType[size];
    std::memcpy(data, other.getData(), sizeInBytes);
    width = other.width;
    height = other.height;

    return *this;
}

ImageData& ImageData::operator=(ImageData&& other)
{
    if (this != &other)
    {
        if (data != nullptr)
            delete [] data;

        data = other.data;
        size = other.size;
        sizeInBytes = other.sizeInBytes;
        width = other.width;
        height = other.height;

        other.data = nullptr;
        other.size = 0u;
        other.sizeInBytes = 0u;
        other.width = 0;
        other.height = 0;
    }

    return *this;
}

const uchar *ImageData::getByteData() { return reinterpret_cast<uchar*>(data); }
GprData::DataType* ImageData::getData() { return data; }
const GprData::DataType* ImageData::getData() const { return data; }
int ImageData::getSize() const { return size; }

int ImageData::getWidth() const { return width; }
int ImageData::getHeight() const { return height; }

int ImageData::getSizeInBytes() const { return sizeInBytes; }

GprData::DataType& ImageData::at(int row, int col)
{
    return data[row * width + col];
}

const GprData::DataType& ImageData::at(int row, int col) const
{
    return data[row * width + col];
}

GprData::DataType& ImageData::operator[](int index)
{
    return data[index];
}

const GprData::DataType& ImageData::operator[](int index) const
{
    return data[index];
}
