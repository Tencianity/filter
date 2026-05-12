#ifndef PNGHELP_H
#define PNGHELP_H

#include <stdio.h>

#include "png.h"

PNGCHUNK pngReadChunk(FILE*);
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK);
BYTE pngVerifyChunk(PNGCHUNK);
char* pngChunkType(PNGCHUNK);
int filterPNG(PNGHEADER, PNGINFOHEADER, char, FILE*, FILE*);
void pngBlur(int, RGB*);
void pngGrayscale(int, RGB*);
void pngReflect(int, RGB*);
void pngSepia(int, RGB*);
void pngRedShift(int, RGB*);
void pngGreenShift(int, RGB*);
void pngBlueShift(int, RGB*);

static inline DWORD pngChunkSize(PNGCHUNK chunk) {
    return (sizeof(chunk.length) + sizeof(chunk.type) + sizeof(chunk.crc)) + reverseLong(chunk.length);
}

static inline DWORD pngTrueLength(PNGCHUNK chunk) {
    return reverseLong(chunk.length);
}

#endif