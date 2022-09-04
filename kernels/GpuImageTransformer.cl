#define USHORT_MAX ((1<<16) - 1)

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
    float gainLower,
	uint rowsPerWorkItem)
{
    size_t id = get_global_id(0);
    size_t groupFrom = from + id * rowsPerWorkItem;
	size_t groupTo = (groupFrom + rowsPerWorkItem) < height ? groupFrom + rowsPerWorkItem : height;

    float gain = gainLower + id * step;
    int ushortMax = USHORT_MAX;
	int ushortHalfMax = ushortMax/2;

    int row, col;
	for (row = groupFrom; row < groupTo; row++)
	{
    	for (col = 0; col < width; col++)
	    {		
		    int newValue = (int)(((int)src[row * width + col] - ushortHalfMax) * gain);
			newValue += ushortHalfMax;
			if (newValue > ushortMax)
			    newValue = ushortMax;
			if (newValue < 0)
			    newValue = 0;
		
    	    dest[row * width + col] = (ushort)newValue;
        }
		gain += step;
    }
}


__kernel void applyFilterInt(
    __global ushort* src,
    __global ushort* dest,
    uint width,
    uint height,
    __global int* mask,
    uint maskHeight,
    uint maskWidth,
	uint rowsPerWorkItem)
{
    int ushortMax = USHORT_MAX;
    size_t midHeight = maskHeight/2;
	size_t midWidth = maskWidth/2;
    size_t row, col, i, j, r, c;
	
    size_t id = get_global_id(0);
    size_t from = midHeight + id * rowsPerWorkItem;
	size_t to = (from + rowsPerWorkItem) <= (height - midHeight) ? (from + rowsPerWorkItem) : (height - midHeight);
	
	int value;
    for (row = from; row < to; row++)
	{
	    for (col = midWidth; col < width - midWidth; col++)
		{
		    value = 0;
			for (i = 0; i < maskHeight; i++)
			{
			    for (j = 0; j < maskWidth; j++)
				{
					r = row + i - midHeight;
					c = col + j - midWidth;
				    value += ((int)(src[r * width + c]) * mask[i * maskWidth + j]);
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

__kernel void backgroundRemoval(
    __global ushort* src,
	__global ushort* dest,
	uint width,
	uint height,
	uint rowsPerWorkItem)
{
    int ushortMax = USHORT_MAX;
    int ushortHalfMax = ushortMax/2;

    size_t id = get_global_id(0);
    size_t from = id * rowsPerWorkItem;
    size_t to = (from + rowsPerWorkItem) < height ? from + rowsPerWorkItem : height;
	
	size_t row, col;
	int sum;
	for (row = from; row < to; row++)
	{
	    for (col = 0u; col < width; col++)
		{
		    sum += ((int)src[row * width + col] - ushortHalfMax);
		}
		int mean = (int)round(((double)sum)/width);;
		
		for (col = 0u; col < width; col++)
		{
		    int value = (int)src[row * width + col] - mean;
			if (value < 0)
                value = 0;
            else if (value > ushortMax)
                value = ushortMax;
			
			dest[row * width + col] = (ushort)value;
		}
	}
}



