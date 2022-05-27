#pragma once
#include <variant>

namespace details
{
template <typename T>
class Mask;
}

using Mask = std::variant<details::Mask<int>, details::Mask<double>>;

class ImageTransformer
{
public:
    virtual ~ImageTransformer() = default;
    virtual void rotate90() = 0;
    virtual void changeContrast(float contrast) = 0;
    virtual void gain(int from, int to, float value) = 0;
    virtual void gain(int from, int to, double gainLower, double gainUpper) = 0;
    virtual void equalizeHistogram(int from, int to) = 0;
    virtual void applyFilter(const Mask&) = 0;
};
