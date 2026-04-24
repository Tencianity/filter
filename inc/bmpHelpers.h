#ifndef BMPHELP_H
#define BMPHELP_H

#include <stdio.h>

#include "bmp.h"

// Read a BITMAPINFOHEADER BMP
int filterBMP(BITMAPFILEHEADER bf, BITMAPV5INFOHEADER bi, char filter, FILE *inptr, FILE *outptr);

//Greyscale filter
void grayscale(int height, int width, RGB image[height][width]);

//Sepia filter
void sepia(int height, int width, RGB image[height][width]);

//Reflect filter
void reflect(int height, int width, RGB image[height][width]);

//Blur filter
void blur(int height, int width, RGB image[height][width]);

// Red filter
void redShift(int height, int width, RGB image[height][width]);

// Green filter
void greenShift(int height, int width, RGB image[height][width]);

// Blue filter
void blueShift(int height, int width, RGB image[height][width]);

char* getInfoHeaderType(int headerSize);

void printHeaderError(char* infoheader);

#endif