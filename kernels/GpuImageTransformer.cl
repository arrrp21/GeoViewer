__kernel void rotate90(__global ushort* src, __global ushort* dest, int newWidth, int newHeight)
{
    int _h = get_global_id(0);
    int _w = get_global_id(0);
    //for (w = 0; w < newWidth; w++)
      //  dest[h * newWidth + w] = src[(newWidth - w - 1) * newHeight + h];

    int h;
    int w;
    for (h = _h * 8; h < _h * 8 + 8; h++)
        for (w = _w * 8; w < _w * 8 + 8; w++)
            if (h < newHeight && w < newWidth)
                dest[h * newWidth + w] = src[(newWidth - w - 1) * newHeight + h];
}

/*for (int h = 0; h < newHeight; h++)
    for (int w = 0; w < newWidth; w++)
        castedNewData[h * newWidth + w] = castedOldData[(newWidth - w - 1) * newHeight + h];*/
