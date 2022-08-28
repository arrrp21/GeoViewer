#pragma once
#include <vector>
#include "GprData.hpp"

class ImageData
{
public:
    ImageData() = default;
    ImageData(int width, int height);
    ImageData(const ImageData&);
    ImageData(ImageData&&) noexcept;
    ImageData& operator=(const ImageData&);
    ImageData& operator=(ImageData&&);
    ImageData(std::vector<GprData::DataType>& dataVector, int width, int height);

    const uchar* getByteData();
    GprData::DataType* getData();
    const GprData::DataType* getData() const;
    GprData::DataType* getDataAt(int row, int col);
    const GprData::DataType* getDataAt(int row, int col) const;
    int getSizeInBytes() const;
    int getSize() const;
    int getWidth() const;
    int getHeight() const;

    GprData::DataType& at(int row, int col);
    const GprData::DataType& at(int row, int col) const;
    GprData::DataType& operator[](int index);
    const GprData::DataType& operator[](int index) const;
    void trimTop(int row);

    ~ImageData();
private:
    GprData::DataType* data = nullptr;
    int size{0};
    int sizeInBytes{0};
    int width{0};
    int height{0};
};
