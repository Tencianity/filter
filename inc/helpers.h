#ifndef HELP_H
#define HELP_H

#include <stdint.h>
#include <string.h>

// Define int-type structs as identifiable names
typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint16_t WORD;

#define TRUE 1
#define FALSE 0

typedef struct {
    LONG x;
    LONG y;
    LONG z;
} __attribute__((__packed__))
CIEXYZ;

// Define how a XYZTRIPLE (pixel) is structured
typedef struct {
	CIEXYZ bluePoint;
	CIEXYZ greenPoint;
	CIEXYZ redPoint;
} __attribute__((__packed__))
XYZTRIPLE;

// Define the struct for a RGB pel
typedef struct {
    BYTE b;
    BYTE g;
    BYTE r;
} __attribute__((__packed__))
RGB;

char* getFileType(char* file);
DWORD reverseLong(DWORD num);
DWORD buildLong(BYTE bytes[4]);

#endif