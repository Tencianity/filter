#include <stdio.h>
#include <stdlib.h>

#include "pngDecodeFilters.h"
#include "helpers.h"

/**
 * Returns the compression score of the selected filter
 * for a given byte stream with widths of <byteWidth>.
 */
int pngFilterScore(BYTE* data, long byteWidth, long offset, FILTERTYPE f) {
    
    BYTE* newData = calloc(byteWidth, sizeof(BYTE));
    if (newData == NULL) {
        printf("Filter score calculation failed.\n");
        return -1;
    }
    
    switch (f) {

        case NONE:
            memcpy(newData, data, byteWidth);
            break;
        case SUB:
            newData = pngSubFilter(data, byteWidth, offset);
            break;
        case UP:
            newData = pngUpFilter(data, byteWidth, offset);
            break;
        case AVERAGE:
            newData = pngAverageFilter(data, byteWidth, offset);
            break;
        case PAETH:
            newData = pngPaethFilter(data, byteWidth, offset);
            break;
        default:
            printf("Invalid chunk filter type.\n");
            return -1;

    }

    int score = 0;
    for (long i = 0; i < byteWidth; i++) {
        score += abs((int) newData[i]);
    }
    
    free(newData);
    return score;
}

/**
 * WIP
 */
BYTE* pngSubFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* subData = calloc(byteWidth, sizeof(BYTE));
    if (subData == NULL) {
        printf("Not enough memory to store sub filtered data.\n");
        return NULL;
    }
    
    for (long i = 0; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];
        BYTE prevByte;
        if (i < 1) prevByte = 0;
        else prevByte = data[offset + i - 1];

        // unsigned int (BYTE) auto applies mod 256.
        subData[i] = (currByte - prevByte);
    }

    return subData;
}

/**
 * WIP
 */
BYTE* pngUnSubFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* unSubData = calloc(byteWidth, sizeof(BYTE));
    if (unSubData == NULL) {
        printf("Not enough memory to store sub unfiltered data.\n");
        return NULL;
    }

    for (long i = 1; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];
        BYTE prevByte;
        if (i - 1 < 1) prevByte = 0;
        else prevByte = unSubData[i - 1];

        // unsigned int (BYTE) auto applies mod 256.
        unSubData[i] = (currByte + prevByte);
    }

    return unSubData;
}

BYTE* pngUpFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* upData = calloc(byteWidth, sizeof(BYTE));
    if (upData == NULL) {
        printf("Not enough memory to store up filtered data.\n");
        return NULL;
    }

    for (long i = 0; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];
        BYTE upByte;
        if (offset < byteWidth) upByte = 0;  // first scanline, no up byte
        else upByte = data[offset + i - byteWidth];

        // unsigned int (BYTE) auto applies mod 256.
        upData[i] = (currByte - upByte);
    }
    
    return upData;
}

BYTE* pngUnUpFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* unUpData = calloc(byteWidth, sizeof(BYTE));
    if (unUpData == NULL) {
        printf("Not enough memory to store up unfiltered data.\n");
        return NULL;
    }

    for (long i = 1; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];
        BYTE upByte;
        if (offset < byteWidth) upByte = 0;  // first scanline, no up byte
        else upByte = unUpData[i - byteWidth];

        // unsigned int (BYTE) auto applies mod 256.
        unUpData[i] = (currByte + upByte);
    }
    
    return unUpData;
}

BYTE* pngAverageFilter(BYTE* data, long byteWidth, long offset) {
    BYTE* avgData = calloc(byteWidth, sizeof(BYTE));
    if (avgData == NULL) {
        printf("Not enough memory to store average filtered data.\n");
        return NULL;
    }

    for (long i = 1; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];
        
        BYTE prevByte;
        if (i - 1 < 1) prevByte = 0;
        else prevByte = data[offset + i - 1];

        BYTE upByte;
        if (offset < byteWidth) upByte = 0;  // first scanline, no up byte
        else upByte = data[offset + i - byteWidth];

        avgData[i] = currByte - (((int) prevByte + (int) upByte) / 2);
    }
    
    return avgData;
}

BYTE* pngUnAverageFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* unAvgData = calloc(byteWidth, sizeof(BYTE));
    if (unAvgData == NULL) {
        printf("Not enough memory to store average filtered data.\n");
        return NULL;
    }

    for (long i = 0; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];
        
        BYTE prevByte;
        if (i < 1) prevByte = 0;
        else prevByte = unAvgData[i - 1];

        BYTE upByte;
        if (offset < byteWidth) upByte = 0;  // first scanline, no up byte
        else upByte = unAvgData[i - byteWidth];  // use reconstructed data from previous row

        unAvgData[i] = currByte + (((int) prevByte + (int) upByte) / 2);
    }
    
    return unAvgData;
}

BYTE* pngPaethFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* paethData = calloc(byteWidth, sizeof(BYTE));
    if (paethData == NULL) {
        printf("Not enough memory to store paeth filtered data.\n");
        return NULL;
    }

    for (long i = 1; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];

        BYTE prevByte;
        if (i - 1 < 1) prevByte = 0;
        else prevByte = data[offset + i - 1];

        BYTE upByte;
        if (offset < byteWidth) upByte = 0;  // first scanline, no up byte
        else upByte = data[offset + i - byteWidth];

        BYTE upLeftByte;
        if (i - 1 < 1 || offset < byteWidth) upLeftByte = 0;  // first row or first column
        else upLeftByte = data[offset + i - byteWidth - 1];

        DWORD v = upByte + prevByte - upLeftByte;
        DWORD vl = v - prevByte;
        DWORD vu = v - upByte;
        DWORD vul = v - upLeftByte;

        if (vl < vu && vl < vul) paethData[i] = currByte - prevByte;
        else if (vu < vul) paethData[i] = currByte - upByte;
        else paethData[i] = currByte - upLeftByte;
    }
    
    return paethData;
}

BYTE* pngUnPaethFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* unPaethData = calloc(byteWidth, sizeof(BYTE));
    if (unPaethData == NULL) {
        printf("Not enough memory to store paeth unfiltered data.\n");
        return NULL;
    }

    for (long i = 0; i < byteWidth; i++) {

        BYTE currByte = data[offset + i];

        BYTE prevByte;
        if (i < 1) prevByte = 0;
        else prevByte = unPaethData[i - 1];

        BYTE upByte;
        if (offset < byteWidth) upByte = 0;  // first scanline, no up byte
        else upByte = unPaethData[i - byteWidth];  // use reconstructed data from previous row

        BYTE upLeftByte;
        if (i < 1 || offset < byteWidth) upLeftByte = 0;  // first row or first column
        else upLeftByte = unPaethData[i - byteWidth - 1];  // use reconstructed data from previous row

        DWORD v = upByte + prevByte - upLeftByte;
        DWORD vl = v - prevByte;
        DWORD vu = v - upByte;
        DWORD vul = v - upLeftByte;

        if (vl < vu && vl < vul) unPaethData[i] = currByte + prevByte;
        else if (vu < vul) unPaethData[i] = currByte + upByte;
        else unPaethData[i] = currByte + upLeftByte;
    }
    
    return unPaethData;
}