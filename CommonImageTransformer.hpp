#pragma once
#include "ImageTransformer.hpp"

class ImageData;
class QImageWrapper;

namespace ImageTransforming
{
class CommonImageTransformer : public ImageTransformer
{
public:
    CommonImageTransformer(QImageWrapper& imageWrapper);

    void rotate90() override;
    void changeContrast(float contrast) override;
    void gain(int from, int to, float value) override;
    void gain(int from, int to, double gainLower, double gainUpper) override;
    void equalizeHistogram(int from, int to) override;
    void applyFilter(const Mask& mask) override;
    void backgroundRemoval() override;

private:
    QImageWrapper& imageWrapper;

    LookupTable createLut(GprData::DataType min, GprData::DataType max);
    LookupTable createLut(float contrast);

    GprData::DataType min(int from, int to, const ImageData& imageData);
    GprData::DataType max(int from, int to, const ImageData& imageData);

    template <class MaskType>
    void applyFilter(const MaskType& mask);
};
} // namespace ImageTransforming
