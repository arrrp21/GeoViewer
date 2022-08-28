#pragma once

#include <chrono>
#include <QDebug>
#include <optional>

class Timer
{
public:
    Timer() {}
    void start(std::optional<QString> = std::nullopt);
    void stop();

    std::chrono::time_point<std::chrono::high_resolution_clock> startPoint{};
    std::optional<QString> m_message{std::nullopt};
};

