#pragma once
#include <limits>

#include "GprData.hpp"
#include "ImageTransformer.hpp"

class QImageWrapper;

using limits = std::numeric_limits<GprData::DataType>;
using LookupTable = std::array<GprData::DataType, limits::max() - limits::min() + 1>;

class CommonImageTransformer : public ImageTransformer
{
public:
    CommonImageTransformer(QImageWrapper& imageWrapper);

    void rotate90() override;
    void changeContrast(float contrast) override;
    void gain(int from, int to, float value) override;
    void equalizeHistogram(int from, int to) override;

private:
    QImageWrapper& imageWrapper;

    LookupTable createLut(GprData::DataType min, GprData::DataType max);
    LookupTable createLut(float contrast);

    GprData::DataType min(int from, int to);
    GprData::DataType max(int from, int to);

};

