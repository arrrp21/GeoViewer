#pragma once

class ImageTransformer
{
public:
    virtual ~ImageTransformer() = default;
    virtual void rotate90() = 0;
    virtual void changeContrast(float contrast) = 0;
};
