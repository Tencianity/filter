#ifndef BMP_H
#define BMP_H

#include "helpers.h"

// Define the file header for bmp image files
typedef struct {
	WORD        signature;
	DWORD       fileSize;
	WORD        reserved1; // 0x00
	WORD        reserved2; // 0x00
	DWORD       dataOffset;
} __attribute__((__packed__))
BITMAPFILEHEADER;

// Define the info header for bmp imgage files
typedef struct {
    DWORD       size;
    LONG        bitWidth;
    LONG        bitHeight;
    WORD        bitPlanes;
    WORD        bitCount; // bpp
    DWORD       compression;
    DWORD       imageSize;
    LONG        xPelsPerMeter;
    LONG        yPelsPerMeter;
    DWORD       clrUsed;
    DWORD       clrImportant;
    DWORD       redMask;
    DWORD       greenMask;
    DWORD       blueMask;
    DWORD       alphaMask;
    DWORD       csType;
    XYZTRIPLE   colorPoint; // This is right
    DWORD       gammaRed;
    DWORD       gammaGreen;
    DWORD       gammeBlue;
    DWORD       intent;
    DWORD       profileData;
    DWORD       profileSize;
    DWORD       reserved3; // 0x00
} __attribute__((__packed__))
BITMAPV5INFOHEADER;

typedef struct {
    DWORD       size;
    LONG        bitWidth;
    LONG        bitHeight;
    WORD        bitPlanes;
    WORD        bitCount; // bpp
} __attribute((__packed__))
BITMAPCOREHEADER;

typedef struct {
    DWORD       size;
    LONG        bitWidth;
    LONG        bitHeight;
    WORD        bitPlanes;
    WORD        bitCount; // bpp
    DWORD       compression;
    DWORD       imageSize;
    LONG        xPelsPerMeter;
    LONG        yPelsPerMeter;
    DWORD       clrUsed;
    DWORD       clrImportant;
} __attribute__((__packed__))
BITMAPINFOHEADER;

// HEADER2 definition taken from;
// https://komh.github.io/os2books/os2tk45/gpi3/048_L2_BITMAPINFOHEADER2.html
typedef struct {
    DWORD       size;
    LONG        bitWidth;
    LONG        bitHeight;
    WORD        bitPlanes;
    WORD        bitCount; // bpp
    DWORD       compression;
    DWORD       imageSize;
    LONG        xPelsPerMeter;
    LONG        yPelsPerMeter;
    DWORD       clrUsed;
    DWORD       clrImportant;
    WORD        units;
    WORD        reserved; // 0x00
    WORD        recording;

    /* Haftoning algs:
     * 1 - Error Diffusion
     * 2 - PANDA
     * 3 - Super-Circle
    */
    WORD        renderingHafton;
    
    DWORD       size1;
    DWORD       size2;
    DWORD       csType;
} __attribute__((__packed__))
BITMAPV2INFOHEADER;

#endif