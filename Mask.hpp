#pragma once

#include <initializer_list>
#include <vector>
#include <QDebug>

namespace image_transforming
{
namespace details
{
template <typename T>
struct Mask
{
    Mask() = default;
    Mask(int size)
        : width{size}
        , height{size}
    {
        values.reserve(size * size);
    }
    Mask(const Mask& other)
        : width{other.width}
        , height{other.height}
        , values{other.values}
    {}
    Mask(Mask&& other) noexcept
        : width{other.width}
        , height{other.height}
        , values{std::move(other.values)}
    {
    }
    Mask& operator=(const Mask& other)
    {
        if (this != &other)
        {
            width = other.width;
            height = other.height;
            values = other.values;
        }
        return *this;
    }
    Mask& operator=(Mask&& other) noexcept
    {
        width = other.width;
        height = other.height;
        values = other.values;

        other.values.clear();
    }

    Mask(int width, int height, std::initializer_list<std::initializer_list<T>> values_)
        : width{width}, height{height}
    {
        values.reserve(width * height);
        for (auto& list : values_)
            for (auto& value : list)
            {
                if (width == 5)
                {
                    qDebug() << "pushing back value: " << value;
                }
                values.push_back(value);
            }
    }

    void add(T value)
    {
        if (values.size() < width * height)
            values.push_back(value);
    }

    const T& at(int row, int col) const
    {
        return values[row * width + col];
    }

    void print() const
    {
        QString str;
        QString str2;
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                str = str + QString::number(at(i, j)) + " ";

        for (auto val : values)
            str2 = str2 + QString::number(val) + " ";

        qDebug() << "Mask width: " << width << " height: " << height << "values at:      " << str;
        qDebug() << "Mask width: " << width << " height: " << height << "values foreach: " << str2;
    }

    int width;
    int height;
    std::vector<T> values;
};
} // namespace details
} // namespace image_transforming
