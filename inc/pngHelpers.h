#ifndef PNGHELP_H
#define PNGHELP_H

#include <stdio.h>

#include "png.h"

PNGCHUNK pngReadChunk(FILE*);
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK);
DWORD pngCalculateCRC(PNGCHUNK);
BYTE pngVerifyChunk(PNGCHUNK);
char* pngChunkType(PNGCHUNK);
int filterPNG(PNGHEADER, PNGINFOHEADER, char, FILE*, FILE*);
RGBA* pngBlur(int, RGBA*);
RGBA* pngGrayscale(int, RGBA*);
RGBA* pngReflect(int, RGBA*);
RGBA* pngSepia(int, RGBA*);
RGBA* pngRedShift(int, RGBA*);
RGBA* pngGreenShift(int, RGBA*);
RGBA* pngBlueShift(int, RGBA*);

static inline DWORD pngChunkSize(PNGCHUNK chunk) {
    return (sizeof(chunk.length) + sizeof(chunk.type) + sizeof(chunk.crc)) + reverseLong(chunk.length);
}

static inline DWORD pngTrueLength(PNGCHUNK chunk) {
    return reverseLong(chunk.length);
}

#endif