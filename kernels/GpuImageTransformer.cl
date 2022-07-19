__kernel void rotate90(__global ushort* src, __global ushort* dest, uint newWidth, uint newHeight, uint workGroupSize)
{
    uint groupWidth = get_global_id(0);
    uint groupHeight = get_global_id(1);

    uint h;
    uint w;
    uint heightStop = min(groupHeight * (workGroupSize) + workGroupSize, newHeight);
    uint widthStop = min(groupWidth * (workGroupSize) + workGroupSize, newWidth);

    for (h = groupHeight * workGroupSize; h < heightStop; h++)
        for (w = groupWidth * workGroupSize; w < widthStop; w++)
            dest[h * newWidth + w] = src[(newWidth - w - 1) * newHeight + h];
}



__kernel void linearGain(
    __global ushort* src,
    __global ushort* dest,
    uint width,
    uint height,
    uint from,
    float step,
    float gainLower)
{
    int groupHeight = get_global_id(0) + from;

    float gain = gainLower + get_global_id(0) * step;
    uint ushortLimit = 1<<16 - 1;

    int w;
    for (w = 0; w < width; w++)
    {
        uint tempNewValue = (uint)((float)src[groupHeight * width + w] * gain);
        ushort newValue = tempNewValue <= ushortLimit ? tempNewValue : ushortLimit;
        dest[groupHeight * width + w] = newValue;
    }
}

