#pragma once
#include <QImage>
#include <cstdint>
#include "GprData.hpp"
#include "Span.hpp"

class QImageWrapper
{
public:
    QImageWrapper(GprData&);
    QImageWrapper() = delete;
    QImageWrapper(const QImageWrapper&) = delete;
    QImageWrapper(QImageWrapper&&) = delete;
    QImageWrapper& operator=(const QImageWrapper&) = delete;
    QImageWrapper& operator=(QImageWrapper&&) = delete;

    GprData::DataType getColor(int x, int y);
    const QImage& getImage() const;
    const Span& getRawData() const;
    const Span& getOriginalRawData() const;
    void setNewImage(QImage&&, Span&&);

private:
    QImage image;
    Span rawData;
    const Span originalRawData;
};

