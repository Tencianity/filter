#include "helpers.h"

char* getFileType(char* file) {

    char ft[4];
    for (int i = 3; i > 0; i--) ft[3-i] = file[strlen(file) - i];
    char* filetype = ft;

    if (!strcmp(filetype, "png") || !strcmp(filetype, "bmp")) return filetype;
    return "Unknown filetype.";
}

/**
 * Reverse the byte order of a long.
 * 
 * @param DWORD The number who's byte order to reverse.
 * @return The reversed DWORD.
 */
DWORD reverseLong(DWORD num) {
    return ((num & 0xff000000) >> 24) |
           ((num & 0x00ff0000) >> 8) |
           ((num & 0x0000ff00) << 8) |
           ((num & 0x000000ff) << 24);
}

DWORD buildLong(BYTE bytes[4]) {
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}