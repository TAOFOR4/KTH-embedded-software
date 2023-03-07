#include "imageProcessing.h"
#include "system.h"

#define ONCHIP_SECTION_1 2048
#define ONCHIP_SECTION_2 4096

unsigned char *g_shared;

unsigned char *grayShared = 0;

void sram2sm_p32(unsigned char *base)
{
	int x, y;
	unsigned char *shared;

	shared = (unsigned char *)SHARED_ONCHIP_BASE;
	g_shared = shared;

	int size_x = *base++;
	int size_y = *base++;
	int max_col = *base++;
	base += size_x * size_y;
	*shared++ = size_x;
	*shared++ = size_y;
	*shared++ = max_col;
	printf("The image is: %d x %d!! \n", size_x, size_y);
	for (y = 0; y < size_y; y++)
		for (x = 0; x < size_x; x++)
		{
			*shared++ = *base++; // R
			*shared++ = *base++; // G
			*shared++ = *base++; // B
		}
}

void graySDF(unsigned char *base)
{
	int x, y;
	unsigned char *shared;
	// printf("\nmesg base1= %s",base);

	shared = base;
	grayShared = (unsigned char *)(SHARED_ONCHIP_BASE + ONCHIP_SECTION_1);
	unsigned char *grayshard2 = grayShared;

	int size_x = *base++;
	int size_y = *base++;
	int max_col = *base++;
	// *shared++  = size_x;
	// *shared++  = size_y;
	// *shared++  = max_col;
	//   shared++; shared++ ; shared++;

	*grayShared++ = size_x;
	*grayShared++ = size_y;
	*grayShared++ = max_col;

	printf("GraySDF - The image is: %d x %d!! \n", size_x, size_y);
	for (y = 0; y < size_y; y++)
		for (x = 0; x < size_x; x++)
		{
			int s = 0;

			// 	*shared++ = *base++ * 0.3125; 	// R
			// shared--;
			s += (int)(*base++ * 0.3125); // R
			// shared++;
			// 	*shared++ = *base++ * 0.5625;	// G
			// shared--;
			s += (int)(*base++ * 0.5625); // G
			// shared++;
			// 	*shared++ = *base++ * 0.125;	// B
			// shared--;
			s += (int)(*base++ * 0.125); // B
			// shared++;
			printf("\nGraySDF - shared-s =  %d  \n", s);
			*grayShared++ = s;
		}
	// printf("\n1-mesg grayShared[0]= %c",grayShared[0]);
	printf("\nw-mesg grayShared[0]= %d %d %d ", grayshard2[0], grayshard2[2], grayshard2[3]);
	grayShared = grayshard2;
	// grayShared = (unsigned char*)(SHARED_ONCHIP_BASE + ONCHIP_SECTION_1);
	// printf("\n2-mesg grayShared[0]= %c",grayShared[0]);
	// printf("\nmesg gray= %s",grayShared);
}