#define USHORT_MAX (1<<16) - 1

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
    int groupFrom = (get_global_id(0) * 16) + from;
	int groupTo = groupFrom + 16 <= height ? groupFrom + 16 : height;

    float gain = gainLower + (get_global_id(0) * 16) * step;
    uint ushortMax = USHORT_MAX;

    int w, h;
	for (h = groupFrom; h < groupTo; h++)
	{
    	for (w = 0; w < width; w++)
	    {
    	    uint tempNewValue = (uint)((float)src[h * width + w] * gain);
            ushort newValue = tempNewValue <= ushortMax ? tempNewValue : ushortMax;
    	    dest[h * width + w] = newValue;
        }
		gain += step;
    }
}


__kernel void applyFilter(
    __global ushort* src,
    __global ushort* dest,
    uint width,
    uint height,
    __global int* mask,
    uint maskWidth,
    uint maskHeight)
{
    uint ushortMax = USHORT_MAX;
    int midHeight = maskHeight/2;
	int midWidth = maskWidth/2;
    int row, col, i, j; 
    for (row = midHeight; row < height - midHeight; row++)
	{
	    for (col = midWidth; col < width - midWidth; col++)
		{
		    int value = 0;
			for (i = 0; i < maskHeight; i++)
			{
			    for (j = 0; j < maskWidth; j++)
				{
					int r = row + i - midHeight;
					int c = col + j - midWidth;
				    value += src[r * width + c] * mask[i * maskWidth + j];
				}
			}
			if (value > ushortMax)
			    value = ushortMax;
			else if (value <= 0)
			    value = 0;
		    dest[row * width + col] = (ushort)value;
		}
	}
}



