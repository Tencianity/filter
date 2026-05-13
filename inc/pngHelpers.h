#ifndef PNGHELP_H
#define PNGHELP_H

#include "png.h"

PNGCHUNK pngReadChunk(FILE*);
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK);
DWORD pngCalculateCRC(PNGCHUNK);
BYTE pngVerifyChunk(PNGCHUNK);
char* pngChunkType(PNGCHUNK);
int filterPNG(PNGHEADER, PNGINFOHEADER, char, FILE*, FILE*);

RGBA* pngPullPixels(BYTE* idatStream, size_t dataSize, BYTE colorType, BYTE bitDepth);
RGBA* pngBlur(RGBA*, size_t, BYTE, BYTE, BYTE);
RGBA* pngGrayscale(RGBA*, size_t, BYTE, BYTE, BYTE);
RGBA* pngReflect(RGBA*, size_t, BYTE, BYTE, BYTE);
RGBA* pngSepia(RGBA*, size_t, BYTE, BYTE, BYTE);
RGBA* pngRedShift(RGBA*, size_t, BYTE, BYTE, BYTE);
RGBA* pngGreenShift(RGBA*, size_t, BYTE, BYTE, BYTE);
RGBA* pngBlueShift(RGBA*, size_t, BYTE, BYTE, BYTE);

static inline DWORD pngChunkSize(PNGCHUNK chunk) {
    return (sizeof(chunk.length) + sizeof(chunk.type) + sizeof(chunk.crc)) + reverseLong(chunk.length);
}

static inline DWORD pngTrueLength(PNGCHUNK chunk) {
    return is_little_endian() ? reverseLong(chunk.length) : chunk.length;
}

#endif