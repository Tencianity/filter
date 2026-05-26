#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "pngImageFilters.h"
#include "helpers.h"
#include "png.h"

RGBA* pngBlur(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {
    const BYTE BLURSIZE = 3;
    RGBA* imageCopy = calloc(width * height, sizeof(RGBA));
    
    for (int i = 0; i < width * height; i++) {
        DWORD redAverage, greenAverage, blueAverage, alphaAverage = 0;

        for (int row = -BLURSIZE; row < BLURSIZE; row++) {
            if (i + width * row < 0 || i + width * row >= width * height) continue; // row above or below image top or bottom

            for (int col = -BLURSIZE; col < BLURSIZE; col++) {
                if (i + col < 0 || i + col >= width * height) continue; // col befoore or after image left or right border
                redAverage += image[i + width * row + col].r;
                greenAverage += image[i + width * row + col].g;
                blueAverage += image[i + width * row + col].b;
                alphaAverage += image[i + width * row + col].a;
            }
        }
        imageCopy[i] = (RGBA) {redAverage, greenAverage, blueAverage, alphaAverage};
    }
    
    memcpy(image, imageCopy, width * height);
    free(imageCopy);
    return image;
}

RGBA* pngGrayscale(RGBA* image, long imageSize, int bytesPerPixel) {
    BYTE* img = (BYTE*) image;

    for (long i = 0; i < imageSize; i++) {
        BYTE r = *(img + 0);
        BYTE g = *(img + 1);
        BYTE b = *(img + 2);
        BYTE a = *(img + 3);
        
        if (a > 0) {
            BYTE average = round((r + g + b) / 3.0);
            *(img + 0) = average;
            *(img + 1) = average;
            *(img + 2) = average;
        }
        img += bytesPerPixel;
    }
    return (RGBA*) image;
}

RGBA* pngReflect(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {
    BYTE* img = (BYTE*) image;
    long byteWidth = width * bytesPerPixel;

    for (DWORD row = 0; row < height; row++) {
        BYTE* left = img;
        BYTE* right = img + byteWidth - bytesPerPixel;

        for (DWORD col = 0; col < width / 2; col++) {
            for (int b = 0; b < bytesPerPixel; b++) {
                left[b] = left[b] ^ right[b];
                right[b] = left[b] ^ right[b];
                left[b] = left[b] ^ right[b];
            }
            left += bytesPerPixel;
            right -= bytesPerPixel;
        }

        img += byteWidth;
    }
    
    return image;
}

RGBA* pngSepia(RGBA* image, long dataSize, int bytesPerPixel) {
    return image;
}

RGBA* pngRedShift(RGBA* image, long dataSize, int bytesPerPixel) {
    return image;
}

RGBA* pngGreenShift(RGBA* image, long dataSize, int bytesPerPixel) {
    return image;
}

RGBA* pngBlueShift(RGBA* image, long dataSize, int bytesPerPixel) {
    return image;
}
