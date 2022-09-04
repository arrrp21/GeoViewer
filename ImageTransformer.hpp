#pragma once
#include <variant>
#include <array>

#include "GprData.hpp"
#include "Operation.hpp"
#include "Timer.hpp"

class ImageData;
class QImageWrapper;

namespace image_transforming
{
namespace details
{
template <typename T>
class Mask;
}

namespace limits
{
static constexpr GprData::DataType min = static_cast<GprData::DataType>(std::numeric_limits<GprData::DataType>::min());
static constexpr GprData::DataType max = static_cast<GprData::DataType>(std::numeric_limits<GprData::DataType>::max());
static constexpr GprData::DataType halfmax = max / 2u;
}

using Mask = std::variant<details::Mask<int>, details::Mask<double>>;
using LookupTable = std::array<GprData::DataType, limits::max - limits::min + 1>;

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

    void equalizeHistogram(QImageWrapper&, int from, int to);
    void trimTop(QImageWrapper&);

    LookupTable createLut(GprData::DataType min, GprData::DataType max);
    GprData::DataType min(int from, int to, const ImageData& imageData);
    GprData::DataType max(int from, int to, const ImageData& imageData);
    void fillEdges(ImageData&, int, int);
    void fillUpperEdge(ImageData&, int);
    void fillRightEdge(ImageData&, int, int);
    void fillLowerEdge(ImageData&, int);
    void fillLeftEdge(ImageData&, int, int);
};
} // namespace image_transforming
