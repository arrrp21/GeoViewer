#pragma once

#include <cstdint>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <ostream>
#include <variant>
#include <QDebug>

struct GprData;
std::variant<GprData, QString> tryCreateGprData(QFile&);

struct GprData
{
    using DataType = std::uint16_t;

    GprData() = delete;
    GprData(double, double, std::uint32_t, std::uint32_t, std::vector<DataType>&&);
    GprData(const GprData&) = delete;
    GprData(GprData&&);
    GprData& operator=(const GprData&) = delete;
    GprData& operator=(GprData&&);

    double RANGE;
    double PROP_VEL;
    std::uint32_t N_ACQ_SAMPLE;
    std::uint32_t N_ACQ_SWEEP;
    double X_STEP;

    std::vector<DataType> data;
};

QDebug operator<<(QDebug debug, const GprData&);
