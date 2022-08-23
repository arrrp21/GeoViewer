#pragma once

#include "Mask.hpp"

namespace image_transforming
{
using Mask = std::variant<details::Mask<int>, details::Mask<double>>;

struct PredefinedMask
{
    QString name;
    Mask mask3x3;
    Mask mask5x5;
};

const details::Mask<int> highPassFilter3x3{3, 3, {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}};
const details::Mask<int> highPassFilter5x5{5, 5, {
    {0,  -1,  -1, -1,  0},
    {-1,  2,  -4,  2, -1},
    {-1, -4,  21, -4, -1},
    {-1,  2,  -4,  2, -1},
    {0,  -1,  -1, -1,  0}}};

// { 0, -1, -1, -1,  0}, {-1,  22  -4,  2, -1}, {-1, -4, 18, -4, -1}, {-1,  2  -4,  2, -1}, { 0, -1, -1, -1,  0}
const PredefinedMask highPassFilter{
    "High pass filter",
    highPassFilter3x3,
    highPassFilter5x5,
};
} // namespace image_transforming
