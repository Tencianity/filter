#include "helpers.h"
#include <math.h>

// Convert image file into greyscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            int avgRed = round((image[i][j].rgbtRed.x + image[i][j].rgbtRed.z) / 2);
            int avgGreen = round((image[i][j].rgbtGreen.x + image[i][j].rgbtGreen.z) / 2);
            int avgBlue = round((image[i][j].rgbtBlue.x + image[i][j].rgbtBlue.z) / 2);

            int average = round((avgRed + avgGreen + avgBlue) / 3.0);

            image[i][j].rgbtRed.x, image[i][j].rgbtRed.z = average;
            image[i][j].rgbtGreen.x, image[i][j].rgbtGreen.z = average;
            image[i][j].rgbtBlue.x, image[i][j].rgbtBlue.z = average;
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