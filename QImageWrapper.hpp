#pragma once
#include <QImage>
#include <cstdint>
#include "GprData.hpp"
#include "ImageData.hpp"

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
    int height() const;
    int width() const;
    const QImage& getImage() const;
    const ImageData& getImageData() const;
    const ImageData& getOriginalImageData() const;
    void changeOriginalImageData(const ImageData&);
    void setNewImage(ImageData&&);
    void resetImage();

private:
    ImageData imageData;
    ImageData originalImageData;
    QImage image;
};

