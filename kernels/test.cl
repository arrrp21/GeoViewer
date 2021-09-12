const sampler_t samp = CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

__kernel void rgbToGray(__read_write image2d_t srcImage)
{
    int2 pos = (get_global_id(0), get_global_id(1));
    uint4 srcColor = read_imageui(srcImage, samp, pos);
    uint gray = srcColor.z * 0.299f +
                 srcColor.y * 0.587f +
                 srcColor.x * 0.114f;
    uint4 pixel = (uint4)(srcColor.xyz * gray, srcColor.w);
    write_imageui(srcImage, pos, pixel);
}

/*__kernel void rgbToGray(__read_write image2d_t srcImage)
{
	printf("siema");
	int x = get_global_id(0);
	int y = get_global_id(1);

	int2 pos = (int2)(x, y);
	if (x <= 2 && y <= 2)
	{
		//printf("x: %d, y: %d,   R: %d, G: %d, B: %d\n", x, y, );
		printf("x: %d, y: %d\n", x, y);
	}
}*/