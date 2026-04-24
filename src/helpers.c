#include "helpers.h"

char* getFileType(char* file) {

    char ft[4];
    for (int i = 3; i > 0; i--) ft[3-i] = file[strlen(file) - i];
    char* filetype = ft;

    if (!strcmp(filetype, "png") || !strcmp(filetype, "bmp")) return filetype;
    return "Unknown filetype.";
}

DWORD reverseLong(DWORD num) {
    DWORD newNum = 0x0;
    for (int i = 1; i <= sizeof(num); i++) {
        int shiftRight = 8 * (sizeof(num) - i);
        int shiftLeft = 8 * (i - 1);
        newNum |= (num << shiftLeft) >> shiftRight;
    }
    return newNum;
}

DWORD buildLong(BYTE bytes[4]) {
    DWORD ret = 0x0;
    for (int i = 0; i < sizeof(DWORD); i++) {
        int shiftAmt = sizeof(DWORD) - i;
        ret |= bytes[i];
        ret <<= shiftAmt; 
    }
    return ret;
}