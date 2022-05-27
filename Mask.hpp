#pragma once

#include <initializer_list>
#include <vector>
#include <QDebug>

namespace details
{
template <typename T>
struct Mask
{
    /*Mask() = delete;
    Mask(const Mask&) = delete;
    Mask(Mask&&) = delete;
    Mask& operator=(const Mask&) = delete;
    Mask& operator=(Mask&&) = delete;*/

    Mask(int width, int height, std::initializer_list<std::initializer_list<T>> values_)
        : width{width}, height{height}
    {
        values.reserve(width * height);
        for (auto& list : values_)
            for (auto& value : list)
                values.push_back(value);
    }

    const T& at(int row, int col) const
    {
        return values[row * width + col];
    }

    int width;
    int height;
    std::vector<T> values;
};
} // namespace details
