#include <cstdint>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <ostream>

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

struct GprData
{
    GprData(QFile& file);

    using DataType = int;

    double RANGE;
    double PROP_VEL;
    std::uint32_t N_ACQ_SAMPLE;
    std::uint32_t N_ACQ_SWEEP;
    double X_STEP;

    std::vector<DataType> data;

private:
    void readData(QTextStreamWrapper&);
};

QDebug operator<<(QDebug debug, const GprData&);
