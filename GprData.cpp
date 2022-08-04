#include "GprData.hpp"
#include <ios>
#include <QTextStream>
#include <QDebug>
#include "fmt/format.h"

class QTextStreamWrapper : public QTextStream
{
public:
    QTextStreamWrapper(QIODevice* device)
        : QTextStream(device) {}
    QString readLine()
    {
        lineCount++;
        return QTextStream::readLine();
    }
    std::uint32_t getLineCount() { return lineCount; }
private:
    std::uint32_t lineCount{0u};
};

std::optional<QString> readData(
    QTextStreamWrapper& inputTextStream,
    std::uint32_t N_ACQ_SAMPLE,
    std::uint32_t N_ACQ_SWEEP,
    std::vector<GprData::DataType>& data)
{
    if (N_ACQ_SAMPLE > 0 and N_ACQ_SWEEP > 0)
        data.reserve(N_ACQ_SAMPLE * N_ACQ_SWEEP);

    bool isOk;
    while (not inputTextStream.atEnd())
    {
        QString strValue{inputTextStream.readLine()};
        GprData::DataType value{static_cast<GprData::DataType>(strValue.toInt(&isOk) + std::numeric_limits<std::int16_t>::max())};
        if (not isOk)
        {
            return QString::fromStdString(fmt::format("Invalid DATA value (line: {})", inputTextStream.getLineCount()));
        }
        data.push_back(value);
    }

    return std::nullopt;
}

std::variant<GprData, QString> tryCreateGprData(QFile& file)
{
    if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "Couldn't open a file";
    }

    QTextStreamWrapper inputTextStream(&file);

    ScanDirection SCAN_DIRECTION = ScanDirection::None;
    double RANGE = 0;
    double PROP_VEL = 0;
    int N_ACQ_SAMPLE = 0;
    int N_ACQ_SWEEP = 0;
    double X_STEP = 0;
    std::vector<GprData::DataType> data;

    while (not inputTextStream.atEnd())
    {
        QString parameterName{inputTextStream.readLine()};

        bool isOk;
        if (parameterName == "SCAN_DIRECTION")
        {
            QString scanDirection{inputTextStream.readLine()};
            if (scanDirection.trimmed() == "L")
                SCAN_DIRECTION = ScanDirection::L;
        }

        if (parameterName == "RANGE")
        {
            QString range{inputTextStream.readLine()};
            RANGE = range.toDouble(&isOk);
            if (not isOk)
            {
                file.close();
                return QString("Invalid RANGE value (line: %1)").arg(inputTextStream.getLineCount());
            }
        }

        if (parameterName == "PROP_VEL")
        {
            QString propVel{inputTextStream.readLine()};
            PROP_VEL = propVel.toDouble(&isOk);
            if (not isOk)
            {
                file.close();
                return QString("Invalid PROP_VEL value (line: %1)").arg(inputTextStream.getLineCount());
            }
        }

        if (parameterName == "N_ACQ_SAMPLE")
        {
            QString nAcqSample{inputTextStream.readLine()};
            N_ACQ_SAMPLE = nAcqSample.toInt(&isOk);
            if (not isOk)
            {
                file.close();
                return QString("Invalid N_ACQ_SAMPLE value (line: %1)").arg(inputTextStream.getLineCount());
            }
        }

        if (parameterName == "N_ACQ_SWEEP")
        {
            QString nAcqSweep{inputTextStream.readLine()};
            N_ACQ_SWEEP = nAcqSweep.toInt(&isOk);
            if (not isOk)
            {
                file.close();
                return QString("Invalid N_ACQ_SWEEP value (line: %1)").arg(inputTextStream.getLineCount());
            }
        }

        if (parameterName == "X_STEP")
        {
            QString xStep{inputTextStream.readLine()};
            qDebug() << xStep;
            X_STEP = xStep.toDouble(&isOk);
            qDebug() << X_STEP;
            qDebug() << isOk;
            if (not isOk)
            {
                file.close();
                return QString("Invalid X_STEP value (line: %1)").arg(inputTextStream.getLineCount());
            }
        }

        if (parameterName == "DATA")
        {
            std::optional<QString> errorString = readData(inputTextStream, N_ACQ_SAMPLE, N_ACQ_SWEEP, data);
            if (errorString)
            {
                file.close();
                return *errorString;
            }
        }
    }

    file.close();
    return GprData(SCAN_DIRECTION, RANGE, PROP_VEL, N_ACQ_SAMPLE, N_ACQ_SWEEP, X_STEP, std::move(data));
}

GprData::GprData(
    ScanDirection SCAN_DIRECTION,
    double RANGE,
    double PROP_VEL,
    int N_ACQ_SAMPLE,
    int N_ACQ_SWEEP,
    double X_STEP,
    std::vector<DataType> &&data)
    : SCAN_DIRECTION{SCAN_DIRECTION}
    , RANGE{RANGE}
    , PROP_VEL{PROP_VEL}
    , N_ACQ_SAMPLE{N_ACQ_SAMPLE}
    , N_ACQ_SWEEP{N_ACQ_SWEEP}
    , X_STEP{X_STEP}
    , data(std::move(data))
{
}

GprData::GprData(GprData&& other)
    : SCAN_DIRECTION{other.SCAN_DIRECTION}
    , RANGE{other.RANGE}
    , PROP_VEL{other.PROP_VEL}
    , N_ACQ_SAMPLE{other.N_ACQ_SAMPLE}
    , N_ACQ_SWEEP{other.N_ACQ_SWEEP}
    , X_STEP{other.X_STEP}
    , data(std::move(other.data))
{
}

GprData& GprData::operator=(GprData&& other)
{
    SCAN_DIRECTION = other.SCAN_DIRECTION;
    RANGE = other.RANGE;
    PROP_VEL = other.PROP_VEL;
    N_ACQ_SAMPLE = other.N_ACQ_SAMPLE;
    N_ACQ_SWEEP = other.N_ACQ_SWEEP;
    X_STEP = other.X_STEP;
    data = std::move(other.data);
}

QDebug operator<<(QDebug debug, const GprData& gprData)
{
    debug << "RANGE: " << gprData.RANGE << " PROP_VEL: " << gprData.PROP_VEL << "N_ACQ_SAMPLE: " << gprData.N_ACQ_SAMPLE
          << "N_ACQ_SWEEP" << gprData.N_ACQ_SWEEP << "X_STEP: " << gprData.X_STEP << "DATA length: " <<gprData.data.size();

    return debug;
}

