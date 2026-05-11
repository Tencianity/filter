#ifndef PNGHELP_H
#define PNGHELP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "png.h"
#include "CRC32.h"

PNGCHUNK pngReadChunk(FILE*);
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK);
BYTE pngVerifyChunk(PNGCHUNK);
char* pngChunkType(PNGCHUNK);
int filterPNG(PNGHEADER, PNGINFOHEADER, char, FILE*, FILE*);

static inline DWORD pngChunkSize(PNGCHUNK chunk) {
    return (sizeof(chunk.length) + sizeof(chunk.type) + sizeof(chunk.crc)) + reverseLong(chunk.length);
}

static inline DWORD pngTrueLength(PNGCHUNK chunk) {
    return reverseLong(chunk.length);
}

#endif