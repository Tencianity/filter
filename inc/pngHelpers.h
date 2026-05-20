#ifndef PNGHELP_H
#define PNGHELP_H

#include "png.h"

PNGCHUNK pngReadChunk(FILE*);
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK);
DWORD pngCalculateCRC(PNGCHUNK);
BYTE pngVerifyChunk(PNGCHUNK);
char* pngChunkType(PNGCHUNK);
int pngBytesPerPixel(BYTE colorType, BYTE bitDepth);
int filterPNG(PNGHEADER, PNGINFOHEADER, char, FILE*, FILE*);

RGBA* pngPullPixels(BYTE* idatStream, size_t dataSize, 
                     DWORD width, DWORD height,
                     BYTE colorType, BYTE bitDepth, long uncompressedSize);

BYTE* pngPushPixels(RGBA* image, long dataSize,
                     BYTE colorType, BYTE bitDepth, BYTE filterType);
void pngEncode(PNGHEADER, PNGINFOHEADER, BYTE, BYTE*, PNGCHUNK*, size_t, long, FILE*);
RGBA* pngBlur(RGBA*, long);
RGBA* pngGrayscale(RGBA*, long);
RGBA* pngReflect(RGBA*, long);
RGBA* pngSepia(RGBA*, long);
RGBA* pngRedShift(RGBA*, long);
RGBA* pngGreenShift(RGBA*, long);
RGBA* pngBlueShift(RGBA*, long);

static inline DWORD pngChunkSize(PNGCHUNK chunk) {
    return (sizeof(chunk.length) + sizeof(chunk.type) + sizeof(chunk.crc)) + reverseLong(chunk.length);
}

static inline DWORD pngTrueLength(PNGCHUNK chunk) {
    return is_little_endian() ? reverseLong(chunk.length) : chunk.length;
}

#endif