#include "bmp.h"

RGB xyz_to_rgb(RGB xyz);

//Greyscale filter
void grayscale(int height, int width, RGB image[height][width]);

//Sepia filter
void sepia(int height, int width, RGB image[height][width]);

//Reflect filter
void reflect(int height, int width, RGB image[height][width]);

//Blur filter
void blur(int height, int width, RGB image[height][width]);

char* getInfoHeaderType(int headerSize);