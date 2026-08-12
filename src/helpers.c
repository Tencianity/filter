#include "helpers.h"

char* getFileType(char* file) {

    char ft[4];
    for (int i = 3; i > 0; i--) ft[3-i] = file[strlen(file) - i];
    char* filetype = ft;

    if (!strcmp(filetype, "png") || !strcmp(filetype, "bmp")) return filetype;
    return "Unknown filetype.";
}

BYTE getFilterNum(char* filter) {
    if (!strcmp(filter, "grayscale") || !strcmp(filter, "--grayscale")
		|| !strcmp(filter, "greyscale") || !strcmp(filter, "--greyscale")) return 'g';
    if (!strcmp(filter, "sepia") || !strcmp(filter, "--sepia")) return 's';
    if (!strcmp(filter, "reflect") || !strcmp(filter, "--reflect")) return 'r';
    if (!strcmp(filter, "blur") || !strcmp(filter, "--blur")) return 'b';
    if (!strcmp(filter, "red") || !strcmp(filter, "--red")) return 'R';
    if (!strcmp(filter, "green") || !strcmp(filter, "--green")) return 'G';
    if (!strcmp(filter, "blue") || !strcmp(filter, "--blue")) return 'B';

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

void copyPixel(BYTE* src, DWORD srcW, DWORD sx, DWORD sy,
                      BYTE* dst, DWORD dstW, DWORD dx, DWORD dy,
                      BYTE bitDepth, int bpp) {
    if (bitDepth >= 8) {
        // Multi-byte or 1-byte per pixel (8-bit Grayscale, 8/16-bit RGB/RGBA, etc.)
        long srcIdx = (sy * srcW + sx) * bpp;
        long dstIdx = (dy * dstW + dx) * bpp;
        memcpy(dst + dstIdx, src + srcIdx, bpp);
    }
    else {
        // Sub-byte bit depths: 1, 2, or 4 bits per pixel (Grayscale & Indexed)
        int pixelsPerByte = 8 / bitDepth;
        BYTE bitMask = (1 << bitDepth) - 1;

        // Extract pixel from source
        long srcByteIdx = (sy * ((srcW * bitDepth + 7) / 8)) + (sx / pixelsPerByte);
        int srcBitOffset = (pixelsPerByte - 1 - (sx % pixelsPerByte)) * bitDepth;
        BYTE pixelVal = (src[srcByteIdx] >> srcBitOffset) & bitMask;

        // Insert pixel into destination canvas
        long dstByteIdx = (dy * ((dstW * bitDepth + 7) / 8)) + (dx / pixelsPerByte);
        int dstBitOffset = (pixelsPerByte - 1 - (dx % pixelsPerByte)) * bitDepth;
        
        dst[dstByteIdx] &= ~(bitMask << dstBitOffset);
        dst[dstByteIdx] |= (pixelVal << dstBitOffset);
    }
}
