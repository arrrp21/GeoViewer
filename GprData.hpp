#pragma once

#include <cstdint>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <ostream>
#include <variant>
#include <QDebug>

struct GprData;
std::variant<GprData, std::string> tryCreateGprData(QFile&);

enum class ScanDirection : std::uint8_t
{
    None,
    L,
};

struct GprData
{
    using DataType = std::uint16_t;

    GprData() = delete;
    GprData(ScanDirection, double, double, int, int, double, std::vector<DataType>&&);
    GprData(const GprData&) = delete;
    GprData(GprData&&);
    GprData& operator=(const GprData&) = delete;
    GprData& operator=(GprData&&);

    ScanDirection SCAN_DIRECTION = ScanDirection::None;
    double RANGE;
    double PROP_VEL;
    int N_ACQ_SAMPLE;
    int N_ACQ_SWEEP;
    double X_STEP;

    std::vector<DataType> data;
};

QDebug operator<<(QDebug debug, const GprData&);
std::ostream& operator<<(std::ostream& os, const GprData&);
