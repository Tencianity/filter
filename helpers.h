#include "bmp.h"

//Greyscale filter
void grayscale(int height, int width, CIEXYZ image[height][width]);

//Sepia filter
void sepia(int height, int width, CIEXYZ image[height][width]);

//Reflect filter
void reflect(int height, int width, CIEXYZ image[height][width]);

//Blur filter
void blur(int height, int width, CIEXYZ image[height][width]);

char* getInfoHeaderType(int headerSize);