#include "helpers.h"
#include <math.h>

// Convert image file into greyscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            int average = round((image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue) / 3);
            image[i][j].rgbtRed = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtBlue = average;
            return;
        }
    }
}

void sepia(int height, int width, RGBTRIPLE image[height][width]) {
    return;
}

void reflect(int height, int width, RGBTRIPLE image[height][width]) {
    return;
}

void blur(int height, int width, RGBTRIPLE image[height][width]) {
    return;
}