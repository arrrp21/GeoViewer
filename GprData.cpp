#include "GprData.hpp"
#include <ios>
#include <QTextStream>
#include <QDebug>

GprData::GprData(QFile& file)
{
    if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw std::ios_base::failure("Couldn't open a file");
    }

    QTextStreamWrapper inputTextStream(&file);

    while (not inputTextStream.atEnd())
    {
        QString parameterName{inputTextStream.readLine()};

        bool isOk;
        if (parameterName == "RANGE")
        {
            QString range{inputTextStream.readLine()};
            RANGE = range.toDouble(&isOk);
            if (not isOk)
            {
                std::string what{
                    QString("Invalid RANGE value (line: %1)").arg(inputTextStream.getLineCount()).toStdString()};
                throw std::invalid_argument(what);
            }
        }

        if (parameterName == "PROP_VEL")
        {
            QString propVel{inputTextStream.readLine()};
            PROP_VEL = propVel.toDouble(&isOk);
            if (not isOk)
            {
                std::string what{
                    QString("Invalid PROP_VEL value (line: %1)").arg(inputTextStream.getLineCount()).toStdString()};
                throw std::invalid_argument(what);
            }
        }

        if (parameterName == "N_ACQ_SAMPLE")
        {
            QString nAcqSample{inputTextStream.readLine()};
            N_ACQ_SAMPLE = nAcqSample.toDouble(&isOk);
            if (not isOk)
            {
                std::string what{
                    QString("Invalid N_ACQ_SAMPLE value (line: %1)").arg(inputTextStream.getLineCount()).toStdString()};
                throw std::invalid_argument(what);
            }
        }

        if (parameterName == "N_ACQ_SWEEP")
        {
            QString nAcqSweep{inputTextStream.readLine()};
            N_ACQ_SWEEP = nAcqSweep.toDouble(&isOk);
            if (not isOk)
            {
                std::string what{
                    QString("Invalid N_ACQ_SWEEP value (line: %1)").arg(inputTextStream.getLineCount()).toStdString()};
                throw std::invalid_argument(what);
            }
        }

        if (parameterName == "X_STEP")
        {
            QString xStep{inputTextStream.readLine()};
            X_STEP = xStep.toDouble(&isOk);
            if (not isOk)
            {
                std::string what{
                    QString("Invalid X_STEP value (line: %1)").arg(inputTextStream.getLineCount()).toStdString()};
                throw std::invalid_argument(what);
            }
        }

        if (parameterName == "DATA")
        {
            readData(inputTextStream);
        }
    }

    file.close();
}

void GprData::readData(QTextStreamWrapper& inputTextStream)
{
    if (N_ACQ_SAMPLE > 0 and N_ACQ_SWEEP > 0)
        data.reserve(N_ACQ_SAMPLE * N_ACQ_SWEEP);

    bool isOk;
    while (not inputTextStream.atEnd())
    {
        QString value{inputTextStream.readLine()};
        data.push_back(value.toInt(&isOk));
        if (not isOk)
        {
            std::string what{
                QString("Invalid DATA value (line: %1)").arg(inputTextStream.getLineCount()).toStdString()};
            throw std::invalid_argument(what);
        }
    }
}

QDebug operator<<(QDebug debug, const GprData& gprData)
{
    debug << "RANGE: " << gprData.RANGE << " PROP_VEL: " << gprData.PROP_VEL << "N_ACQ_SAMPLE: " << gprData.N_ACQ_SAMPLE
          << "N_ACQ_SWEEP" << gprData.N_ACQ_SWEEP << "X_STEP: " << gprData.X_STEP << "DATA length: " <<gprData.data.size();

    return debug;
}
