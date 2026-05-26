#ifndef DECF_H
#define DECF_H

#include "helpers.h"

typedef enum { NONE = 0, SUB = 1, UP = 2, AVERAGE = 3, PAETH = 4 } FILTERTYPE;

int pngFilterScore(BYTE* data, long byteWidth, long offset, int bpp, FILTERTYPE f);
BYTE* pngSubFilter(BYTE* data, long byteWidth, long offset, int bpp);
BYTE* pngUnSubFilter(BYTE* data, long byteWidth, long offset, int bpp);
BYTE* pngUpFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngUnUpFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngAverageFilter(BYTE* data, long byteWidth, long offset, int bpp);
BYTE* pngUnAverageFilter(BYTE* data, long byteWidth, long offset, int bpp);
BYTE* pngPaethFilter(BYTE* data, long byteWidth, long offset, int bpp);
BYTE* pngUnPaethFilter(BYTE* data, long byteWidth, long offset, int bpp);

#endif