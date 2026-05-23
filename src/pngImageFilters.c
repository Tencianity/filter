#include <stdio.h>
#include <math.h>

#include "pngImageFilters.h"
#include "helpers.h"
#include "png.h"

RGBA* pngBlur(RGBA* image, long dataSize, int bytesPerPixel) {
    return image;
}

RGBA* pngGrayscale(RGBA* image, long dataSize, int bytesPerPixel) {
    BYTE* img = (BYTE*) image;

    for (long i = 0; i < dataSize / bytesPerPixel; i++) {
        BYTE r, g, b, a;
        r = *(img + 0);
        g = *(img + 1);
        b = *(img + 2);
        a = *(img + 3);
        
        // printf("(i: %d, ds: %d)\n", i, dataSize);

        if (a <= 0) continue;
        BYTE average = round((r + g + b) / 3);

        *(img + 0) = average;
        *(img + 1) = average;
        *(img + 2) = average;
        img += bytesPerPixel;
    }
    return image;
}

RGBA* pngReflect(RGBA* image, DWORD width, DWORD height, long dataSize, int bytesPerPixel) {
    // for (int i = 0; i < (height); i++) {
    //     for (int j = 0; j < (width / 2) - (width % 2); j++) {

    //         RGB original = image[i][j];
    //         image[i][j] = image[i][width-j];
    //         image[i][width-j] = original;

    //     }
    // }
    
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
