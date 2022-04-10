#include "Span.hpp"

Span::Span(std::vector<GprData::DataType>& dataVector)
{
    qDebug() << "span ctor";
    const std::size_t sizeInBytes{dataVector.size() * sizeof(GprData::DataType)};
    data = new uchar[sizeInBytes];
    size = sizeInBytes;
    std::memcpy(data, dataVector.data(), sizeInBytes);
}

uchar& Span::operator[](std::size_t index)
{
    return data[index];
}

Span::Span(std::size_t sizeInBytes)
    : size(sizeInBytes)
{
    data = new uchar[sizeInBytes];
}

Span::Span(Span&& other) noexcept
    : data{other.data}, size(other.size)
{
    other.data = nullptr;
    other.size = 0;
}

Span& Span::operator=(Span&& other)
{
    if (this != &other)
    {
        if (data != nullptr)
            delete [] data;

        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;
    }

    return *this;
}

