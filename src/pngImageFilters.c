#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "pngImageFilters.h"
#include "helpers.h"
#include "png.h"

// RGBA* pngBlur(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {

//     const long BLURSIZE = 3;

//     RGBA* imageCopy = calloc(width * height, sizeof(RGBA));
//     if (imageCopy == NULL) {
//         printf("Unable to allocate enough space to copy image pixels.\n");
//         return NULL;
//     }

//     printf("\n\nStarting blur loop.\n"); // debugging

//     if (bytesPerPixel == 4) {
//         for (long y = 0; y < height; y++) {
//             for (long x = 0; x < width; x++) {
//                 if (y == 1 && x == 0) printf("Got to first scanline of pixels for blur Loop.\n"); // debugging
//                 DWORD redAverage = 0, greenAverage = 0, blueAverage = 0, alphaAverage = 0, count = 0;
//                 long pxlIndex = (y * width) + x;

//                 for (long row = -BLURSIZE; row <= BLURSIZE; row++) {
//                     for (long col = -BLURSIZE; col <= BLURSIZE; col++) {

//                         if (y + row < 0 || y + row >= height) continue;
//                         if (x + col < 0 || x + col >= width) continue;

//                         // if (y == 1 && x == 0) printf("Did not break: x: %ld, y: %ld, row: %ld, col: %ld\n", x, y, row, col);

//                         long tempIndex = pxlIndex + (row * width) + col;
                        
//                         redAverage += image[tempIndex].r;
//                         greenAverage += image[tempIndex].g;
//                         blueAverage += image[tempIndex].b;
//                         alphaAverage += image[tempIndex].a;
//                         count++;
//                     }
//                 }

//                 if (count == 0) continue;
//                 redAverage /= (float) count;
//                 greenAverage /= (float) count;
//                 blueAverage /= (float) count;
//                 alphaAverage /= (float) count;

//                 imageCopy[pxlIndex].r = redAverage < 255 ? (BYTE) redAverage : 255;
//                 imageCopy[pxlIndex].g = greenAverage < 255 ? (BYTE) greenAverage : 255;
//                 imageCopy[pxlIndex].b = blueAverage < 255 ? (BYTE) blueAverage : 255;
//                 imageCopy[pxlIndex].a = alphaAverage < 255 ? (BYTE) alphaAverage : 255;
//                 if (y == 0 && x == 0) printf("Got through first pixel for blur Loop.\n"); // debugging
//                 if (y == 1 && x == 0) printf("Got through first scanline of pixels for blur Loop.\n"); // debugging
//                 if (y == height / 2 && x == width / 2) printf("Got through half of pixels for blur Loop.\n"); // debugging
//                 if (y == height - 1 && x == 0) printf("On second last scanline of pixels for blur Loop.\n"); // debugging
//             }
//         }
//     }
    
//     memcpy(image, imageCopy, width * height * sizeof(RGBA));
//     free(imageCopy);
//     return image;
// }

RGBA* pngBlur(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {
    
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

RGBA* pngGrayscale(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {
    
    long imageSize = width * height;
    BYTE* img = (BYTE*) image;

    for (long i = 0; i < imageSize; i++) {
        BYTE r = *(img + 0);
        BYTE g = *(img + 1);
        BYTE b = *(img + 2);
        BYTE a = *(img + 3);
        
        if (a > 0) {
            BYTE average = (r + g + b) / 3;
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

RGBA* pngSepia(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {
    
    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            BYTE* r = &image[i*width + j].r;
            BYTE* g = &image[i*width + j].g;
            BYTE* b = &image[i*width + j].b;

            if (bytesPerPixel == 4) {
                DWORD tr = round((.393f * (float) (*r)) + (.769f * (float) (*g)) + (.189f * (float) (*b)));
                DWORD tg = round((.349f * (float) (*r)) + (.686f * (float) (*g)) + (.168f * (float) (*b)));
                DWORD tb = round((.272f * (float) (*r)) + (.534f * (float) (*g)) + (.131f * (float) (*b)));

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
        }
    }
    
    return image;
}

RGBA* pngRedShift(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {
    
    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            long p = i * width + j;
            BYTE* r = &image[p].r;
            BYTE* g = &image[p].g;
            BYTE* b = &image[p].b;

            if (bytesPerPixel == 4) {
                DWORD tr = *r * 1.20f;
                DWORD tg = *g * 0.90f;
                DWORD tb = *b * 0.90f;

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
        }
    }
    
    return image;
}

RGBA* pngGreenShift(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {

    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            long p = i * width + j;
            BYTE* r = &image[p].r;
            BYTE* g = &image[p].g;
            BYTE* b = &image[p].b;

            if (bytesPerPixel == 4) {
                DWORD tr = *r * 0.90f;
                DWORD tg = *g * 1.20f;
                DWORD tb = *b * 0.90f;

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
        }
    }
    
    return image;
}

RGBA* pngBlueShift(RGBA* image, DWORD width, DWORD height, int bytesPerPixel) {

    for (long i = 0; i < height; i++) {
        for (long j = 0; j < width; j++) {

            long p = i * width + j;
            BYTE* r = &image[p].r;
            BYTE* g = &image[p].g;
            BYTE* b = &image[p].b;

            if (bytesPerPixel == 4) {
                DWORD tr = *r * 0.90f;
                DWORD tg = *g * 0.90f;
                DWORD tb = *b * 1.20f;

                *r = tr <= 255 ? (BYTE) tr : 255;
                *g = tg <= 255 ? (BYTE) tg : 255;
                *b = tb <= 255 ? (BYTE) tb : 255;
            }
        }
    }
    
    return image;
}
