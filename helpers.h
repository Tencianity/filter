#include "bmp.h"
#include <stdio.h>

// Read a BITMAPINFOHEADER BMP
int readBMIH(BITMAPFILEHEADER bf, BITMAPINFOHEADER bi, char filter, FILE *inptr, FILE *outptr);

// Read a BITMAPCOREHEADER BMP
int readBMCH(BITMAPFILEHEADER bf, BITMAPCOREHEADER bi, char filter, FILE *inptr, FILE *outptr);

// Read a BITMAPV2INFOHEADER BMP
int readBMIH2(BITMAPFILEHEADER bf, BITMAPV2INFOHEADER bi, char filter, FILE *inptr, FILE *outptr);

// Read a BITMAPV5INFOHEADER BMP
int readBMIH5(BITMAPFILEHEADER bf, BITMAPV5INFOHEADER bi, char filter, FILE *inptr, FILE *outptr);

//Greyscale filter
void grayscale(int height, int width, RGB image[height][width]);

//Sepia filter
void sepia(int height, int width, RGB image[height][width]);

//Reflect filter
void reflect(int height, int width, RGB image[height][width]);

//Blur filter
void blur(int height, int width, RGB image[height][width]);

char* getInfoHeaderType(int headerSize);