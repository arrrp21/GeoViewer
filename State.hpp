#pragma once
#include "ImageData.hpp"
#include "Operation.hpp"

struct State
{
    int sliderRangeLowerValue;
    int sliderRangeUpperValue;
    int sliderGainLowerValue;
    int sliderGainUpperValue;
    bool isTopTrimmed;
    ImageData imageData;
    image_transforming::Operation operation;
};

