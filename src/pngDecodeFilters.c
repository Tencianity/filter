#include <stdio.h>
#include <stdlib.h>

#include "pngDecodeFilters.h"
#include "helpers.h"

/**
 * Returns the compression score of the selected filter
 * for a given byte stream with widths of <byteWidth>.
 */
int pngFilterScore(BYTE* data, long byteWidth, long offset, FILTERTYPE f) {
    
    switch (f) {

        case NONE:
            data = data;
            break;
        case SUB:
            data = pngSubFilter(data, byteWidth, offset);
            break;
        case UP:
            data = pngUpFilter(data, byteWidth, offset);
            break;
        case AVERAGE:
            data = pngAverageFilter(data, byteWidth, offset);
            break;
        case PAETH:
            data = pngPaethFilter(data, byteWidth, offset);
            break;
        default:
            printf("Invalid chunk filter type.\n");
            return -1;

    }

    if (data == NULL) {
        printf("Filter score calculation failed.\n");
        return -1;
    }
    
    int score = 0;
    for (int i = 0; i < byteWidth; i++) {
        score += abs((int) data[i]);
    }
    
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
    
    for (int i = 0; i < byteWidth; i++) {

        BYTE currPel = data[offset + i];
        BYTE prevPel;
        if (i - 1 < 1) prevPel = 0;
        else prevPel = data[offset + i - 1];

        // unsigned int (BYTE) auto applies mod 256.
        subData[i] = (currPel - prevPel);
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

    for (int i = 1; i < byteWidth; i++) {

        BYTE subPel = data[offset + i];
        BYTE prevPel;
        if (i - 1 < 1) prevPel = 0;
        else prevPel = unSubData[i - 1];

        // unsigned int (BYTE) auto applies mod 256.
        unSubData[i] = (subPel + prevPel);
    }

    return unSubData;
}

BYTE* pngUpFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* upData = calloc(byteWidth, sizeof(BYTE));
    if (upData == NULL) {
        printf("Not enough memory to store up filtered data.\n");
        return NULL;
    }

    for (int i = 0; i < byteWidth; i++) {

        BYTE currPel = data[offset + i];
        BYTE upPel;
        if (byteWidth - offset < 0) upPel = 0;
        else upPel = data[offset + i - byteWidth];

        // unsigned int (BYTE) auto applies mod 256.
        upData[i] = (currPel - upPel);
    }
    
    return upData;
}

BYTE* pngUnUpFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* unUpData = calloc(byteWidth, sizeof(BYTE));
    if (unUpData == NULL) {
        printf("Not enough memory to store up unfiltered data.\n");
        return NULL;
    }

    for (int i = 1; i < byteWidth; i++) {

        BYTE currPel = data[offset + i];
        BYTE upPel;
        if (byteWidth - offset < 1) upPel = 0;
        else upPel = data[offset + i - byteWidth];

        // unsigned int (BYTE) auto applies mod 256.
        unUpData[i] = (currPel + upPel);
    }
    
    return unUpData;
}

BYTE* pngAverageFilter(BYTE* data, long byteWidth, long offset) {
    BYTE* avgData = calloc(byteWidth, sizeof(BYTE));
    if (avgData == NULL) {
        printf("Not enough memory to store average filtered data.\n");
        return NULL;
    }

    for (int i = 0; i < byteWidth; i++) {

        BYTE currPel = data[offset + i];
        
        BYTE prevPel;
        if (i - 1 < 0) prevPel = 0;
        else prevPel = data[offset + i - 1];

        BYTE upPel;
        if (byteWidth - offset < 0) upPel = 0;
        else upPel = data[offset + i - byteWidth];

        avgData[i] = currPel - (((int) prevPel + (int) upPel) / 2);
    }
    
    return avgData;
}

BYTE* pngUnAverageFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* unAvgData = calloc(byteWidth, sizeof(BYTE));
    if (unAvgData == NULL) {
        printf("Not enough memory to store average filtered data.\n");
        return NULL;
    }

    for (int i = 1; i < byteWidth; i++) {

        BYTE currPel = data[offset + i];
        
        BYTE prevPel;
        if (i - 1 < 1) prevPel = 0;
        else prevPel = unAvgData[i - 1];

        BYTE upPel;
        if (byteWidth - offset < 1) upPel = 0;
        else upPel = data[offset + i - byteWidth];

        unAvgData[i] = currPel + (((int) prevPel + (int) upPel) / 2);
    }
    
    return unAvgData;
}

BYTE* pngPaethFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* paethData = calloc(byteWidth, sizeof(BYTE));
    if (paethData == NULL) {
        printf("Not enough memory to store paeth filtered data.\n");
        return NULL;
    }

    for (int i = 0; i < byteWidth; i++) {

        BYTE currPel = data[offset + i];

        BYTE prevPel;
        if (i - 1 < 0) prevPel = 0;
        else prevPel = data[offset + i - 1];

        BYTE upPel;
        if (byteWidth - offset < 0) upPel = 0;
        else upPel = data[offset + i - byteWidth];

        BYTE upLeftPel;
        if (i - 1 < 0 || byteWidth - offset < 0) upLeftPel = 0;
        else upLeftPel = data[offset + i - byteWidth - 1];

        DWORD v = upPel + prevPel - upLeftPel;
        DWORD vl = v - prevPel;
        DWORD vu = v - upPel;
        DWORD vul = v - upLeftPel;

        if (vl < vu && vl < vul) paethData[i] = currPel - prevPel;
        else if (vu < vul) paethData[i] = currPel - upPel;
        else paethData[i] = currPel - upLeftPel;
    }
    
    return paethData;
}

BYTE* pngUnPaethFilter(BYTE* data, long byteWidth, long offset) {

    BYTE* unPaethData = calloc(byteWidth, sizeof(BYTE));
    if (unPaethData == NULL) {
        printf("Not enough memory to store paeth unfiltered data.\n");
        return NULL;
    }

    for (int i = 1; i < byteWidth; i++) {

        BYTE currPel = data[offset + i];

        BYTE prevPel;
        if (i - 1 < 1) prevPel = 0;
        else prevPel = unPaethData[i - 1];

        BYTE upPel;
        if (byteWidth - offset < 1) upPel = 0;
        else upPel = data[offset + i - byteWidth];

        BYTE upLeftPel;
        if (i - 1 < 1 || byteWidth - offset < 1) upLeftPel = 0;
        else upLeftPel = data[offset + i - byteWidth - 1];

        DWORD v = upPel + prevPel - upLeftPel;
        DWORD vl = v - prevPel;
        DWORD vu = v - upPel;
        DWORD vul = v - upLeftPel;

        if (vl < vu && vl < vul) unPaethData[i] = currPel + prevPel;
        else if (vu < vul) unPaethData[i] = currPel + upPel;
        else unPaethData[i] = currPel + upLeftPel;
    }
    
    return unPaethData;
}