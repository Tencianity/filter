#ifndef DECF_H
#define DECF_H

#include "helpers.h"

typedef enum { NONE = 0, SUB = 1, UP = 2, AVERAGE = 3, PAETH = 4 } FILTERTYPE;

int pngFilterScore(BYTE* data, long byteWidth, long offset, FILTERTYPE f);
BYTE* pngSubFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngUnSubFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngUpFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngUnUpFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngAverageFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngUnAverageFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngPaethFilter(BYTE* data, long byteWidth, long offset);
BYTE* pngUnPaethFilter(BYTE* data, long byteWidth, long offset);

#endif