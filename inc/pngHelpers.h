#ifndef PNGHELP_H
#define PNGHELP_H

#include "png.h"
#include "helpers.h"

PNGCHUNK pngReadChunk(FILE*);
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK);
DWORD pngCalculateCRC(PNGCHUNK);
BYTE pngVerifyChunk(PNGCHUNK);
char* pngChunkType(PNGCHUNK);
int pngBytesPerPixel(BYTE colorType, BYTE bitDepth);
int filterPNG(PNGHEADER, PNGINFOHEADER, char, FILE*, FILE*);

RGBA* pngPullPixels(BYTE* idatStream, long dataSize, 
                     DWORD width, DWORD height,
                     BYTE colorType, BYTE bitDepth, BYTE interlace);

DATASTREAM pngPushPixels(RGBA* image, long dataSize,
                     DWORD width, DWORD height,
                     BYTE colorType, BYTE bitDepth);
void pngEncode(PNGHEADER, PNGINFOHEADER, BYTE, DATASTREAM, PNGCHUNK*, DWORD, FILE*);
RGBA* pngUnfilter(BYTE* imageStream, DWORD width, DWORD height);
RGBA* pngUnlace(RGBA* image, DWORD witdh, DWORD height);

static inline DWORD pngChunkSize(PNGCHUNK chunk) {
    return (sizeof(chunk.length) + sizeof(chunk.type) + sizeof(chunk.crc)) + reverseLong(chunk.length);
}

static inline DWORD pngTrueLength(PNGCHUNK chunk) {
    return is_little_endian() ? reverseLong(chunk.length) : chunk.length;
}

#endif