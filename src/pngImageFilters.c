#include <stdio.h>
#include <math.h>

#include "pngImageFilters.h"
#include "helpers.h"
#include "png.h"

RGBA* pngBlur(RGBA* image, long dataSize, int bytesPerPixel) {
    return image;
}

RGBA* pngGrayscale(RGBA* image, long dataSize, int bytesPerPixel) {

    for (long i = 0; i < dataSize / bytesPerPixel; i++) {
        
        // printf("(i: %d, ds: %d)\n", i, dataSize);

        if (image[i].a == 0) continue;
        BYTE average = round((image[i].r + image[i].g + image[i].b) / 3);

        image[i].r = average;
        image[i].g = average;
        image[i].b = average;
    }
    return image;
}

RGBA* pngReflect(RGBA* image, long dataSize, int bytesPerPixel) {
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
