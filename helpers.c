#include "helpers.h"
#include <math.h>
#include <stdio.h>

// Convert image file into greyscale
void grayscale(int height, int width, CIEXYZ image[height][width])
{
    printf("\nFirst and second pixel xyz before: (%d, %d, %d), (%d, %d, %d)", image[0][0].x, image[0][0].y, image[0][0].z, image[1][1].x, image[1][1].y, image[1][1].z);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            
            image[i][j].x = 0x00;
            image[i][j].z = 0x00;
        }
    }

    printf("\nFirst and second pixel xyz after: (%d, %d, %d), (%d, %d, %d)\n\n", image[0][0].x, image[0][0].y, image[0][0].z, image[1][1].x, image[1][1].y, image[1][1].z);

    return;
}

void sepia(int height, int width, CIEXYZ image[height][width]) {
    return;
}

void reflect(int height, int width, CIEXYZ image[height][width]) {
    return;
}

void blur(int height, int width, CIEXYZ image[height][width]) {
    return;
}

char* getInfoHeaderType(int headerSize) {
    switch (headerSize)
    {
        case 12:
            return "BITMAPCOREHEADER";
        case 64:
            return "BITMAPCOREHEADER2";
        case 16:
            return "OS22XBITMAPHEADER";
        case 40:
            return "BITMAPINFOHEADER";
        case 52:
            return "BITMAPV2INFOHEADER";
        case 56:
            return "BITMAPV3INFOHEADER";
        case 108:
            return "BITMAPV4HEADER";
        case 124:
            return "BITMAPV5HEADER";
        default:
            return "UNKOWN";
    }
    
}