#pragma once
#include <QtGlobal>
#include <vector>
#include <cstring>
#include "GprData.hpp"

class Span
{
public:
    Span() = delete;
    Span(std::size_t);
    Span(const Span&) = delete;
    Span(Span&&) noexcept;
    Span& operator=(const Span&) = delete;
    Span& operator=(Span&&);
    Span(std::vector<GprData::DataType>& dataVector);

    const uchar* getByteData();
    GprData::DataType* getData();
    const GprData::DataType* getData() const;
    std::size_t getSizeInBytes() const;
    std::size_t getSize() const;

    uchar& operator[](std::size_t index);

    ~Span();
private:
    uchar* data = nullptr;
    std::size_t sizeInBytes{0};
};
