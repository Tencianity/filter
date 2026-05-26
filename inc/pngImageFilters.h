#ifndef PNGIMG_H
#define PNGIMG_H

#include "helpers.h"
#include "png.h"

RGBA* pngBlur(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngGrayscale(RGBA* image, long dataSize, int bytesPerPixel);
RGBA* pngReflect(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngSepia(RGBA* image, long dataSize, int bytesPerPixel);
RGBA* pngRedShift(RGBA* image, long dataSize, int bytesPerPixel);
RGBA* pngGreenShift(RGBA* image, long dataSize, int bytesPerPixel);
RGBA* pngBlueShift(RGBA* image, long dataSize, int bytesPerPixel);

#endif