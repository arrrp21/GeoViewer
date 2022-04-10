#pragma once
#include "ImageTransformer.hpp"

class QImageWrapper;

class CommonImageTransformer : public ImageTransformer
{
public:
    CommonImageTransformer(QImageWrapper& imageWrapper);

    void rotate90() override;

    QImageWrapper& imageWrapper;
};

