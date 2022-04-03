__kernel void rgb2gray(__global uchar* src, __global uchar* dest, int width, int height)
{
	const int x = 4 * get_global_id(0);
	const int y = 4 * get_global_id(1);
	
	uchar color = (uchar)(0.299f * src[y * width + x] +
						  0.587f * src[y * width + x + 1] +
					      0.114f * src[y * width + x + 2]);
	
	dest[y * width + x] = dest[y * width + x + 1] = dest[y * width + x + 2] = color;
	dest[y * width + x + 3] = 255;
}