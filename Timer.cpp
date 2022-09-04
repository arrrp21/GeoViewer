#include "Timer.hpp"
#include <iostream>
#include <sstream>
#include "Log.hpp"

void Timer::start(std::optional<std::string> message)
{
    if (message)
    {
        m_message = QString::fromStdString(message.value());
    }
    isMeasurementOngoing = true;
    startPoint = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
    if (not isMeasurementOngoing)
    {
        LOG_WRN("Timer::stop called when time measurement not started!");
        return;
    }

    const auto stopPoint = std::chrono::high_resolution_clock::now();
    auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(stopPoint - startPoint);

    std::ostringstream out;
    out << executionTime.count();
    const QString duration = QString::fromStdString(out.str());
    QString msg = "";
    if (m_message)
    {
        msg = m_message.value() + " Execution time: " + duration + "ms";
        m_message = std::nullopt;
    }
    else
    {
        msg = "Execution time: " + duration;
    }
    qDebug() << msg;
    isMeasurementOngoing = false;
}
