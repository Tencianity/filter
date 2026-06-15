#ifndef BMPHELP_H
#define BMPHELP_H

#include <stdio.h>

#include "bmp.h"

// Read a BITMAPINFOHEADER BMP
int filterBMP(BITMAPFILEHEADER bf, BITMAPV5INFOHEADER bi, char filter[100], FILE *inptr, FILE *outptr);

char* getInfoHeaderType(int headerSize);

void printHeaderError(char* infoheader);

#endif