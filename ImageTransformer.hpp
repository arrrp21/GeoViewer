#pragma once

class ImageTransformer
{
public:
    virtual ~ImageTransformer() = default;
    virtual void rotate90() = 0;
    virtual void changeContrast(float contrast) = 0;
    virtual void gain(int from, int to, float value) = 0;
    virtual void equalizeHistogram(int from, int to) = 0;
};
