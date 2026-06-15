#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "bmpHelpers.h"
#include "bmpImageFilters.h"

/*
 * Convert an array of RGB pels to their grayscale values.
 */
void grayscale(int height, int width, RGB image[height][width]) {

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            BYTE average = round((image[i][j].r + image[i][j].g + image[i][j].b) / 3);

            image[i][j].r = average;
            image[i][j].g = average;
            image[i][j].b = average;

        }
    }

    return;
}

/**
 * Apply a sepia color filter to an array of RGB pels.
 */
void sepia(int height, int width, RGB image[height][width]) {

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            int tr = round((.393 * image[i][j].r) + (.769 * image[i][j].g) + (.189 * image[i][j].b));
            int tg = round((.349 * image[i][j].r) + (.686 * image[i][j].g) + (.168 * image[i][j].b));
            int tb = round((.272 * image[i][j].r) + (.534 * image[i][j].g) + (.131 * image[i][j].b));

            if (tr > 255) tr = 255;
            if (tg > 255) tg = 255;
            if (tb > 255) tb = 255;
            
            image[i][j].r = tr;
            image[i][j].g = tg;
            image[i][j].b = tb;
            
        }
    }
    
    return;
}

/**
 * Reflects an array of RGB pels about the y-axis.
 */
void reflect(int height, int width, RGB image[height][width]) {
    
    for (int i = 0; i < (height); i++) {
        for (int j = 0; j < (width / 2) - (width % 2); j++) {

            RGB original = image[i][j];
            image[i][j] = image[i][width-j];
            image[i][width-j] = original;

        }
    }
    
    return;
}

/**
 * Applies a blur filter to an array of RGB pels.
 */
void blur(int height, int width, RGB image[height][width]) {
    
    RGB(*newImage)[width] = calloc(height, width * sizeof(RGB));
    int sumRed, sumGreen, sumBlue, pels;
    int i, j;
    int blurriness = 3;

    
    i = 0; j = 0;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            
            sumRed = 0; sumGreen = 0; sumBlue = 0; pels = 0;

            // Take 3x3 slice of pel array and average
            // the RGB values: apply to center pel.
            for (int row = -blurriness; row <= blurriness; row++) {
                for (int col = -blurriness; col <= blurriness; col++) {

                    if (i + row < 0 || i + row >= height
                        || j + col < 0 || j + col >= width)
                        {continue;}

                    sumRed += image[i + row][j + col].r;
                    sumGreen += image[i + row][j + col].g;
                    sumBlue += image[i + row][j + col].b;
                    pels++;
                    
                }
            }

            newImage[i][j].r = round(sumRed / pels);
            newImage[i][j].g = round(sumGreen / pels);
            newImage[i][j].b = round(sumBlue / pels);
        }
    }

    i = 0; j = 0;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {

            image[i][j].r = newImage[i][j].r;
            image[i][j].g = newImage[i][j].g;
            image[i][j].b = newImage[i][j].b;

        }
    }
    free(newImage);
    
    return;
}

void redShift(int height, int width, RGB image[height][width]) {

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int tr = image[i][j].r * 1.20f;
            int tg = image[i][j].g * 0.90f;
            int tb = image[i][j].b * 0.90f;
            
            if (tr > 255) tr = 255;
            if (tg > 255) tg = 255;
            if (tb > 255) tb = 255;

            
            image[i][j].r = (int) tr;
            image[i][j].g = (int) tg;
            image[i][j].b = (int) tb;
        }
    }
    return;
}

void greenShift(int height, int width, RGB image[height][width]) {

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int tr = image[i][j].r * 0.90f;
            int tg = image[i][j].g * 1.20f;
            int tb = image[i][j].b * 0.90f;
            
            if (tr > 255) tr = 255;
            if (tg > 255) tg = 255;
            if (tb > 255) tb = 255;

            
            image[i][j].r = (int) tr;
            image[i][j].g = (int) tg;
            image[i][j].b = (int) tb;
        }
    }
    return;
}

void blueShift(int height, int width, RGB image[height][width]) {

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int tr = image[i][j].r * 0.90f;
            int tg = image[i][j].g * 0.90f;
            int tb = image[i][j].b * 1.20f;
            
            if (tr > 255) tr = 255;
            if (tg > 255) tg = 255;
            if (tb > 255) tb = 255;

            
            image[i][j].r = (int) tr;
            image[i][j].g = (int) tg;
            image[i][j].b = (int) tb;
        }
    }
    return;
}