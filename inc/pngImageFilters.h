#ifndef PNGIMG_H
#define PNGIMG_H

#include "helpers.h"
#include "png.h"

RGBA* pngBlur(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngGrayscale(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngReflect(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngSepia(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngRedShift(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngGreenShift(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);
RGBA* pngBlueShift(RGBA* image, DWORD width, DWORD height, int bytesPerPixel);

#endif