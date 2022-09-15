#pragma once

#include <chrono>
#include <QDebug>
#include <optional>

class Timer
{
public:
    Timer() {}
    void start(std::optional<std::string> = std::nullopt);
    void stop();

private:
    std::chrono::time_point<std::chrono::steady_clock> startPoint{};
    std::optional<QString> m_message{std::nullopt};
    bool isMeasurementOngoing{false};
};

