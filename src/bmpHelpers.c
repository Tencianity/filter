#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpHelpers.h"
#include "bmpImageFilters.h"
#include "helpers.h"

// Read a BITMAPINFOHEADER BMP
int filterBMP(BITMAPFILEHEADER bf, BITMAPV5INFOHEADER bi,
                char filter[100], FILE *inptr, FILE *outptr) {

    int headerSize = bf.dataOffset - (14 * sizeof(BYTE));

    // Read infile's INFOHEADER
    fread(&bi, headerSize, 1, inptr);
    

    // Ensure infile is (likely) an uncompressed BMP
    if (bf.signature != 0x4d42 || bi.bitCount != 24 || bi.compression != 0) {
        fclose(outptr);
        fclose(inptr);
        
        printf("Unsupported file format.\n\n");
        printf("bfType: %x, bfOffBits: %d, biSize: %d, biBitCount: %d",
             bf.signature, bf.dataOffset, bi.size, bi.bitCount);

        printf("Expected BITMAPFILEHEADER size: %li\n", sizeof(BITMAPFILEHEADER));
        printf("Input BITMAPFILEHEADER size: %u\n", bf.fileSize);
        
        // return 6;
    }
    printf("\n\nsizeof(infoheader): %lu\n", sizeof(bi));
    printf("headersize: %d\n\n", headerSize);

    // Get image's dimensions
    int height = abs(bi.bitHeight);
    int width = abs(bi.bitWidth);

    // Allocate memory for image
    RGB(*image)[width] = calloc(height, width * sizeof(RGB));
    if (image == NULL) {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    // int padding = (4 - (width * sizeof(RGB)) % 4) % 4;
    int padding = (width * sizeof(RGB)) % 4;

    printf("Reading input .bmp file...\n");
    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++) {
        // Read row into pixel array
        fread(image[i], sizeof(RGB), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }
    printf("done reading.\n");

    printf("Filtering pixels of image...\n");

    // Count number of image filters being applied;
    int numFilters = 0;
    while (filter[numFilters++] != -1);
    for (int i = 0; i < numFilters; i++) {
        BYTE f = filter[i];
        switch (f) {
            // Blur
            case BLUR:
                blur(height, width, image);
                break;

            // Grayscale
            case GRAYSCALE:
                grayscale(height, width, image);
                break;

            // Reflect
            case REFLECT:
                reflect(height, width, image);
                break;

            // Sepia
            case SEPIA:
                sepia(height, width, image);
                break;

            // Red
            case REDSHIFT:
                redShift(height, width, image);
                break;

            // Green
            case GREENSHIFT:
                greenShift(height, width, image);
                break;

            // Blue
            case BLUESHIFT:
                blueShift(height, width, image);
                break;
        }
    }
    printf("Done filtering image pixels.\n");

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's INFOHEADER
    fwrite(&bi, headerSize, 1, outptr);

    printf("Writing filtered pixels to outfile...\n");
    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outfile
        fwrite(image[i], sizeof(RGB), width, outptr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }
    printf("Done writing to outfile.\n\n");

    printf("\n~~~~ BITMAPFILEHEADER ~~~~\n");

    printf("Signature: %x, File Size: %u, reserved1: %d, reserved2: %d, Offset: %d\n\n",
        bf.signature, bf.fileSize, bf.reserved1, bf.reserved2, bf.dataOffset);

    // Free memory for image
    free(image);
    return 0;
}

char* getInfoHeaderType(int headerSize) {

    switch (headerSize)
    {
        case 12:
            return "BITMAPCOREHEADER";
        case 64:
            return "BITMAPCOREHEADER2";
        case 16:
            return "OS22XBITMAPHEADER";
        case 40:
            return "BITMAPINFOHEADER";
        case 52:
            return "BITMAPV2INFOHEADER";
        case 56:
            return "BITMAPV3INFOHEADER";
        case 108:
            return "BITMAPV4HEADER";
        case 124:
            return "BITMAPV5HEADER";
        default:
            return "UNKOWN";
    }
}

void printHeaderError(char* infoheader) {

    printf("\nInfo Header is of type %s, filter only accepts:\n", infoheader);
    printf("-\tBITMAPCOREHEADER\n");
    printf("-\tBITMAPINFOHEADER\n");
    printf("-\tBITMAPV2INFOHEADER\n");
}