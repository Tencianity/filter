#ifndef BMPIMG_H
#define BMPIMG_H

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

#endif