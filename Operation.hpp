#pragma once

#include <cstdint>

namespace image_transforming
{
enum class Operation : std::uint8_t
{
    none = 0,
    equalizeHist = 1,
    gain = 2,
    gamma = 3
};
} // namespace image_processing
