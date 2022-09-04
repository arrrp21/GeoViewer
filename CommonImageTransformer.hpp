#pragma once
#include "ImageTransformer.hpp"
#include "Timer.hpp"

class ImageData;
class QImageWrapper;

namespace image_transforming
{
class CommonImageTransformer : public ImageTransformer
{
public:
    CommonImageTransformer(QImageWrapper& imageWrapper);

    void rotate90() override;
    void gain(int from, int to, double gainLower, double gainUpper) override;
    void equalizeHistogram(int from, int to) override;
    void applyFilter(const Mask& mask) override;
    void backgroundRemoval() override;
    void trimTop() override;

    void commitChanges(Operation) override {}

private:
    QImageWrapper& imageWrapper;

    template <class MaskType>
    void applyFilter(const MaskType& mask);
};
} // namespace image_transforming
