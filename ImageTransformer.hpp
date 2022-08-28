#pragma once
#include <variant>
#include <array>

#include "GprData.hpp"
#include "Operation.hpp"
#include "Timer.hpp"

namespace image_transforming
{
namespace details
{
template <typename T>
class Mask;
}

using Mask = std::variant<details::Mask<int>, details::Mask<double>>;
using limits = std::numeric_limits<GprData::DataType>;
using LookupTable = std::array<GprData::DataType, limits::max() - limits::min() + 1>;

class ImageTransformer
{
public:
    virtual ~ImageTransformer() = default;
    virtual void rotate90() = 0;
    virtual void gain(int from, int to, double gainLower, double gainUpper) = 0;
    virtual void equalizeHistogram(int from, int to) = 0;
    virtual void applyFilter(const Mask&) = 0;
    virtual void backgroundRemoval() = 0;
    virtual void trimTop() = 0;

    virtual void commitChanges(Operation) = 0;

protected:
    Timer timer{};
};
} // namespace image_transforming
