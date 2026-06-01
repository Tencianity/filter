#ifndef PNGIMG_H
#define PNGIMG_H

#include "helpers.h"
#include "png.h"

RGBA* pngBlur(RGBA* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
RGBA* pngGrayscale(RGBA* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
RGBA* pngReflect(RGBA* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
RGBA* pngSepia(RGBA* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
RGBA* pngRedShift(RGBA* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
RGBA* pngGreenShift(RGBA* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
RGBA* pngBlueShift(RGBA* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);

#endif