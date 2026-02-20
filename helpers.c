#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

RGB rgb_to_xyz(RGB xyz)
{
    RGB chroma;
    chroma.r = 0.0f;
    chroma.g = 0.0f;
    chroma.b = 0.0f;
    return chroma;
}

// Convert image file into greyscale
void grayscale(int height, int width, RGB image[height][width])
{

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            BYTE average = round((image[i][j].r + image[i][j].g + image[i][j].b) / 3);

            image[i][j].r = average;
            image[i][j].g = average;
            image[i][j].b = average;

        }
    }

    return;
}

/**
 * Applies a Sepia filter to the image.
 */
void sepia(int height, int width, RGB image[height][width]) 
{

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            float tr = (0.393f * (float)image[i][j].r) + (0.769f * (float)image[i][j].g) + (0.189f * (float)image[i][j].b);
            float tg = (0.349f * (float)image[i][j].r) + (0.686f * (float)image[i][j].g) + (0.168f * (float)image[i][j].b);
            float tb = (0.272f * (float)image[i][j].r) + (0.534f * (float)image[i][j].g) + (0.131f * image[i][j].b);

            if (tr > 255) tr = 255;
            if (tg > 255) tr = 255;
            if (tb > 255) tb = 255;
            
            image[i][j].r = (int) round(tr);
            image[i][j].g = (int) round(tg);
            image[i][j].b = (int) round(tb);
            
        }
    }
    
    return;
}

/**
 * Flips the image about the y-axis.
 */
void reflect(int height, int width, RGB image[height][width]) 
{
    
    for (int i = 0; i < (height) - (height % 2); i++) {
        for (int j = 0; j < (width / 2) - (width % 2); j++) {

            RGB original = image[i][j];
            image[i][j] = image[i][width-j];
            image[i][width-j] = original;

        }
    }
    
    return;
}

/**
 * Applies a blur filter to the image.
 */
void blur(int height, int width, RGB image[height][width]) 
{
    RGB (*newImage)[width] = calloc(height, width * sizeof(RGB));
    RGB sum;
    RGB average;
    int i, j, pixles;

    for (i = 0; i < height; i += 3) {
        for (j = 0; j < width; j += 3) {

            sum.r = 0; sum.g = 0; sum.b = 0;
            average.r = 0; average.g = 0; average.b = 0;
            pixles = 1;

            // Take 3x3 slice of pixel array and average
            // the RGB values: apply to all 9 pixels.
            for (int row = -1; row <= 1; row++) {
                if (i + row < 0 || i + row >= height) continue;
                for (int col = -1; col <= 1; col++) {
                    if (j + col < 0 || j + col >= width) continue;

                    sum.r += image[row][col].r;
                    sum.g += image[row][col].g;
                    sum.b += image[row][col].b;
                    pixles++;
                    
                }
            }
            average.r = (int) round(sum.r / pixles);
            average.g = (int) round(sum.g / pixles);
            average.b = (int) round(sum.b / pixles);
            
            newImage[height][width].r = average.r;
            newImage[height][width].g = average.g;
            newImage[height][width].b = average.b;
        }
    }
    
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {

            image[i][j].r = newImage[i][j].r;
            image[i][j].g = newImage[i][j].g;
            image[i][j].b = newImage[i][j].b;

        }
    }
    free(newImage);
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