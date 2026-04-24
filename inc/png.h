#ifndef PNG_H
#define PNG_H

#include "helpers.h"

typedef struct {
    BYTE highBit; // 0x89
    char png[3]; // 50 5E 47 ("PNG")
    WORD dos; // 0D 0A | IDK what this means
    BYTE endOfFile; // 1A | IADK what this means
    BYTE lineEnding; // 0A
} __attribute__((__packed__))
PNGHEADER;

typedef struct {
    DWORD length;
    char type[4];
    DWORD width;
    DWORD height;
    BYTE bitDepth; // 1, 2, 4, 8, or 16
    BYTE colorType; // 0, 2, 3, 4, or 6
    BYTE compression; // 0
    BYTE filter; // 0
    BYTE interlace; // 0
    DWORD crc;
} __attribute__((__packed__))
PNGINFOHEADER;

typedef struct {
    DWORD length;
    char type[4];
    BYTE* data;
    DWORD crc;
} __attribute__((__packed__))
PNGCHUNK;

#endif