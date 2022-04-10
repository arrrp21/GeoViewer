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

    const QImage& getImage();
    void setNewImage(QImage&&, Span&&);

private:
    QImage image;
    Span rawData;
};

