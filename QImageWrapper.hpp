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

    GprData::DataType getColor(int x, int y) const;
    int height() const;
    int width() const;
    const QImage& getImage() const;
    const ImageData& getImageData() const;
    const ImageData& getPreviousImageData() const;
    const ImageData& getOriginalImageData() const;
    void updateOriginalImage();
    void updatePreviousImage();
    void setNewImage(ImageData&&);
    void resetImage();

private:
    ImageData imageData;
    ImageData originalImageData;
    ImageData previousImageData;
    QImage image;
};

