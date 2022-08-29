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
    int groupFrom = (get_global_id(0) * rowsPerWorkItem) + from;
	int groupTo = groupFrom + rowsPerWorkItem <= height ? groupFrom + rowsPerWorkItem : height;

    float gain = gainLower + (get_global_id(0) * rowsPerWorkItem) * step;
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
    uint ushortMax = USHORT_MAX;
    size_t midHeight = maskHeight/2;
	size_t midWidth = maskWidth/2;
    size_t row, col, i, j;
	
    size_t id = get_global_id(0);
    size_t from = midHeight + id * rowsPerWorkItem;
	size_t to = (from + rowsPerWorkItem) <= (height - midHeight) ? (from + rowsPerWorkItem) : (height - midHeight);
	
	/*int mask11 = mask[0];
	int mask12 = mask[1];
	int mask13 = mask[2];
	int mask21 = mask[3];
	int mask22 = mask[4];
	int mask23 = mask[5];
	int mask31 = mask[6];
	int mask32 = mask[7];
	int mask33 = mask[8];*/

    for (row = from; row < to; row++)
	{
	    for (col = midWidth; col < width - midWidth; col++)
		{
		    int value = 0;
			
			/*int r, c;
			
			r = row - 1;
			c = col - 1;
			value += src[r * width + c] * mask11;
			
			c++;
			value += src[r * width + c] * mask12;
			
			c++;
			value += src[r * width + c] * mask13;
			
			
			r++;
			c = col - 1;
			value += src[r * width + c] * mask21;
			c++;
			value += src[r * width + c] * mask22;
			c++;
			value += src[r * width + c] * mask23;
			
			r++;
			c = col - 1;
			value += src[r * width + c] * mask31;
			c++;
			value += src[r * width + c] * mask32;
			c++;
			value += src[r * width + c] * mask33;*/

			
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



