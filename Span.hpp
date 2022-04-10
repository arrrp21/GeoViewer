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

    uchar* getData() { return data; }
    std::size_t getSize() { return size; }

    uchar& operator[](std::size_t index);

    ~Span()
    {
        delete [] data;
    }
private:
    uchar* data = nullptr;
    std::size_t size{0};
};
