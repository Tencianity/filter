#ifndef PNGIMG_H
#define PNGIMG_H

#include "helpers.h"
#include "png.h"

BYTE* pngBlur(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
BYTE* pngGrayscale(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
BYTE* pngReflect(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
BYTE* pngSepia(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
BYTE* pngRedShift(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
BYTE* pngGreenShift(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);
BYTE* pngBlueShift(BYTE* image, DWORD width, DWORD height, BYTE bitDepth, BYTE colorType);

#endif