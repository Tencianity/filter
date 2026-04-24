#ifndef PNGHELP_H
#define PNGHELP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"
#include "CRC32.h"

PNGCHUNK pngReadChunk(FILE* pngFile);
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK chunk);
char* pngChunkType(PNGCHUNK chunk);
int filterPNG(PNGHEADER pf, PNGINFOHEADER pi, char filter, FILE* inptr, FILE* outptr);

static inline DWORD pngChunkSize(PNGCHUNK chunk) {
    return (sizeof(chunk.length) + sizeof(chunk.type) + sizeof(chunk.crc)) + reverseLong(chunk.length);
}

#endif