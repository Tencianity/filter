#include "helpers.h"

char* getFileType(char* file) {

    char ft[4];
    for (int i = 3; i > 0; i--) ft[3-i] = file[strlen(file) - i];
    char* filetype = ft;

    if (!strcmp(filetype, "png") || !strcmp(filetype, "bmp")) return filetype;
    return "Unknown filetype.";
}

BYTE getFilterNum(char* filter) {
    if (!strcmp(filter, "grayscale") || !strcmp(filter, "--grayscale")) return 'g';
    if (!strcmp(filter, "sepia") || !strcmp(filter, "--sepia")) return 's';
    if (!strcmp(filter, "reflect") || !strcmp(filter, "--reflect")) return 'r';
    if (!strcmp(filter, "blur") || !strcmp(filter, "--blur")) return 'b';
    if (!strcmp(filter, "redshift") || !strcmp(filter, "--redshift")) return 'R';
    if (!strcmp(filter, "greenshift") || !strcmp(filter, "--greenshift")) return 'G';
    if (!strcmp(filter, "blueshift") || !strcmp(filter, "--blueshift")) return 'B';

    return 0;
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

/**
 * Check if system is little or big endian.
 * 
 * @return 1 if little endian, 0 if big endian.
 */
int is_little_endian() {
    uint16_t num = 0x1;
    return (*(uint8_t *)&num == 0x1); // 1 = little-endian, 0 = big-endian
}