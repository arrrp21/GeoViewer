#pragma once

#include <QImage>
#include <QDebug>
#include <algorithm>
#include <GprData.hpp>

#include <iostream>
QImage gprDataToQImage(GprData& gprData)
{
    std::vector<std::uint16_t> data(gprData.data.size());

    std::transform(gprData.data.begin(), gprData.data.end(), data.begin(), [](GprData::DataType value) -> std::uint16_t {
        //return static_cast<std::uint16_t>(static_cast<std::int32_t>(value) + std::numeric_limits<std::int16_t>::max());
        return static_cast<uint16_t>(value);
    });

    const std::uint16_t min = *std::min_element(data.cbegin(), data.cend());
    const std::uint16_t max = *std::max_element(data.cbegin(), data.cend());

    qDebug() << "max: " << max << ", min: " << min;

    return QImage(reinterpret_cast<uchar*>(data.data()), gprData.N_ACQ_SAMPLE, gprData.N_ACQ_SWEEP, gprData.N_ACQ_SAMPLE * 2, QImage::Format_Grayscale16);
}
