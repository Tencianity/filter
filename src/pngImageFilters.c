#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "pngHelpers.h"
#include "pngImageFilters.h"
#include "helpers.h"
#include "png.h"

BYTE* pngBlur(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType) {
    
    int bytesPerPixel = pngBytesPerPixel(colorType, bitDepth);
    const BYTE BLURSIZE = 3;
    long byteWidth = width * bytesPerPixel;
    
    BYTE* img = (BYTE*) image;
    BYTE* imageCopy = calloc(byteWidth * height, sizeof(BYTE));
    if (imageCopy == NULL) {
        printf("Unable to allocate space for copy of image pixels.\n");
        return NULL;
    }
    
    // Loops over every sample byte of the image and averages the BLURSIZExBLURSIZE
    // sum of sample bytes around it
    for (long i = 0; i < byteWidth * height; i++) {
        DWORD byteAverage = 0;
        int count = 0;

        for (int row = -BLURSIZE; row <= BLURSIZE; row++) {
            
            long exactRow = i / byteWidth + row;
            if (exactRow < 0 || exactRow >= height) continue; // row above or below image top or bottom

            for (int col = -BLURSIZE; col <= BLURSIZE; col++) {
                
                long exactCol = (i % byteWidth) + (col * bytesPerPixel);
                if (exactCol < 0 || exactCol >= byteWidth) continue; // col before or after image left or right border
                
                BYTE sample = img[exactRow * byteWidth + exactCol];
                byteAverage += sample;
                count++;
            }
        }
        imageCopy[i] = (BYTE) (byteAverage / count);
    }
    
    memcpy(img, imageCopy, byteWidth * height);
    free(imageCopy);
    return image;
}

BYTE* pngGrayscale(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType) {
    
    // BYTE bytesPerPixel = pngBytesPerPixel(colorType, bitDepth);
    // long imageSize = width * height;
    // BYTE* img = (BYTE*) image;

    // for (long i = 0; i < imageSize; i++) {
    //     BYTE r = *(img + 0);
    //     BYTE g = *(img + 1);
    //     BYTE b = *(img + 2);
    //     BYTE a = colorType == 4 || colorType == 6 ? *(img + 3) : 255;
        
    //     if (a > 0) {
    //         BYTE average = (r + g + b) / 3;
    //         *(img + 0) = average;
    //         *(img + 1) = average;
    //         *(img + 2) = average;
    //     }
    //     img += bytesPerPixel;
    // }
    return image;
}

BYTE* pngReflect(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType) {
    
    int bytesPerPixel = pngBytesPerPixel(colorType, bitDepth);
    BYTE* img = (BYTE*) image;
    long byteWidth = width * bytesPerPixel;

    for (DWORD row = 0; row < height; row++) {
        BYTE* left = img;
        BYTE* right = img + byteWidth - bytesPerPixel;

        for (DWORD col = 0; col < width / 2; col++) {
            for (int b = 0; b < bytesPerPixel; b++) {

                // Simple value-swap equation with bit manipulation
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

BYTE* pngSepia(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType) {
    
    int bytesPerPixel = pngBytesPerPixel(colorType, bitDepth);
    BYTE* img = (BYTE*) image;

    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            BYTE* r = img + 0;
            BYTE* g = img + 1;
            BYTE* b = img + 2;

            if (bytesPerPixel == 4) {
                DWORD tr = round((.393f * (float) (*r)) + (.769f * (float) (*g)) + (.189f * (float) (*b)));
                DWORD tg = round((.349f * (float) (*r)) + (.686f * (float) (*g)) + (.168f * (float) (*b)));
                DWORD tb = round((.272f * (float) (*r)) + (.534f * (float) (*g)) + (.131f * (float) (*b)));

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
            img += bytesPerPixel;
        }
    }
    
    return image;
}

BYTE* pngRedShift(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType) {
    
    int bytesPerPixel = pngBytesPerPixel(colorType, bitDepth);
    BYTE* img = (BYTE*) image;

    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            BYTE* r = img + 0;
            BYTE* g = img + 1;
            BYTE* b = img + 2;

            if (bytesPerPixel == 4) {
                DWORD tr = *r * 1.20f;
                DWORD tg = *g * 0.90f;
                DWORD tb = *b * 0.90f;

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
            img += bytesPerPixel;
        }
    }
    
    return image;
}

BYTE* pngGreenShift(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType) {

    int bytesPerPixel = pngBytesPerPixel(colorType, bitDepth);
    BYTE* img = (BYTE*) image;

    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            BYTE* r = img + 0;
            BYTE* g = img + 1;
            BYTE* b = img + 2;

            if (bytesPerPixel == 4) {
                DWORD tr = *r * 0.90f;
                DWORD tg = *g * 1.20f;
                DWORD tb = *b * 0.90f;

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
            img += bytesPerPixel;
        }
    }
    
    return image;
}

BYTE* pngBlueShift(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType) {

    int bytesPerPixel = pngBytesPerPixel(colorType, bitDepth);
    BYTE* img = (BYTE*) image;

    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            BYTE* r = img + 0;
            BYTE* g = img + 1;
            BYTE* b = img + 2;

            if (bytesPerPixel == 4) {
                DWORD tr = *r * 0.90f;
                DWORD tg = *g * 0.90f;
                DWORD tb = *b * 1.20f;

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
            img += bytesPerPixel;
        }
    }
    
    return image;
}
