#include "Span.hpp"

Span::Span(std::vector<GprData::DataType>& dataVector)
    : sizeInBytes{dataVector.size() * sizeof(GprData::DataType)}
{
    data = new uchar[sizeInBytes];
    std::memcpy(data, dataVector.data(), sizeInBytes);
}

const uchar *Span::getByteData() { return data; }
GprData::DataType *Span::getData() { return reinterpret_cast<GprData::DataType*>(data); }
const GprData::DataType *Span::getData() const { return reinterpret_cast<const GprData::DataType*>(data); }
std::size_t Span::getSizeInBytes() const { return sizeInBytes; }
std::size_t Span::getSize() const { return sizeInBytes / sizeof(GprData::DataType); }

uchar& Span::operator[](std::size_t index)
{
    return data[index];
}

Span::~Span()
{
    delete [] data;
    sizeInBytes = 0u;
}

Span::Span(std::size_t sizeInBytes)
    : sizeInBytes(sizeInBytes)
{
    data = new uchar[sizeInBytes];
}

Span::Span(Span&& other) noexcept
    : data{other.data}, sizeInBytes(other.sizeInBytes)
{
    other.data = nullptr;
    other.sizeInBytes = 0u;
}

Span& Span::operator=(Span&& other)
{
    if (this != &other)
    {
        if (data != nullptr)
            delete [] data;

        data = other.data;
        sizeInBytes = other.sizeInBytes;

        other.data = nullptr;
        other.sizeInBytes = 0;
    }

    return *this;
}

