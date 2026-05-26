#include <stdio.h>
#include <math.h>

#include "pngImageFilters.h"
#include "helpers.h"
#include "png.h"

RGBA* pngBlur(RGBA* image, long dataSize, int bytesPerPixel) {
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

RGBA* pngReflect(RGBA* image, DWORD width, DWORD height, long dataSize, int bytesPerPixel) {

    // long byteWidth = width * bytesPerPixel;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // long pelOffset = col * bytesPerPixel;
            
        
        }
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
