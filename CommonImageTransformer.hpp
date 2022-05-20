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

private:
    QImageWrapper& imageWrapper;


    LookupTable createLut(float contrast);
};

