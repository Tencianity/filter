#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Read a BITMAPINFOHEADER BMP
int readBMIH(BITMAPFILEHEADER bf, BITMAPINFOHEADER bi, char filter, FILE *inptr, FILE *outptr)
{
    int headerSize = bf.dataOffset - (14 * sizeof(BYTE));

    // Read infile's INFOHEADER
    fread(&bi, headerSize, 1, inptr);
    

    // Ensure infile is (likely) an uncompressed BMP
    if (bf.signature != 0x4d42 || bi.bitCount != 24 || bi.compression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        
        printf("Unsupported file format.\n\n");
        printf("bfType: %x, bfOffBits: %d, biSize: %d, biBitCount: %d, biCompression: %d", bf.signature, bf.dataOffset, bi.size, bi.bitCount, bi.compression);

        printf("Expected BITMAPFILEHEADER size: %li\n", sizeof(BITMAPFILEHEADER));
        printf("Input BITMAPFILEHEADER size: %u\n", bf.fileSize);
        
        // return 6;
    }
    printf("\n\nsizeof(infoheader): %lu\n\n", sizeof(bi));

    // Get image's dimensions
    int height = abs(bi.bitHeight);
    int width = abs(bi.bitWidth);

    // Allocate memory for image
    RGB(*image)[width] = calloc(height, width * sizeof(RGB));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGB)) % 4) % 4;

    printf("Reading input .bmp file...\n");
    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGB), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }
    printf("done reading.\n");


    printf("Filtering pixels of image...\n");
    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflection
        case 'r':
            reflect(height, width, image);
            break;

        // Sepia
        case 's':
            sepia(height, width, image);
            break;
    }
    printf("done filtering image pixels.\n");

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's INFOHEADER
    fwrite(&bi, sizeof(bi), 1, outptr);

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
    printf("doen writing to outfile.\n\n");

    char* compressionType = "";
    switch (bi.compression) {
        case 0: 
            compressionType = "None"; 
            break;
        case 1: 
            compressionType = "BI_RLE8";
            break;
        case 2: 
            compressionType = "BI_RLE4";
            break;
        case 3: 
            compressionType = "BI_BITFIELDS";
            break;
        case 4: 
            compressionType = "BI_JPEG";
            break;
        case 5: 
            compressionType = "BI_PNG"; 
            break;
        case 6: 
            compressionType = "BO_ALPHABITFIELDS"; 
            break;
        case 11: 
            compressionType = "BI_CMYK";
            break;
        case 12: 
            compressionType = "BO_CMYKRLE8"; 
            break;
        case 13: 
            compressionType = "BI_CMYKRLE4"; 
            break;
        default: 
            compressionType = "Unkown";
    }

    printf("\n~~~~ BITMAPFILEHEADER ~~~~\n");

    printf("Signature: %x, File Size: %u, reserved1: %d, reserved2: %d, Offset: %d\n\n",
        bf.signature, bf.fileSize, bf.reserved1, bf.reserved2, bf.dataOffset);

    printf("\n ~~~~ BITMAPINFOHEADER ~~~~\n");

    printf("Compression Bits: %x, Compression Type: %s\n\n", bi.compression, compressionType);
    // Free memory for image
    free(image);

    // Close files
    fclose(inptr);
    fclose(outptr);
    return 0;

}

// Read a BITMAPCOREHEADER BMP
int readBMCH(BITMAPFILEHEADER bf, BITMAPCOREHEADER bi, char filter, FILE *inptr, FILE *outptr)
{

    int headerSize = bf.dataOffset - (14 * sizeof(BYTE));

    // Read infile's INFOHEADER
    fread(&bi, headerSize, 1, inptr);
    

    // Ensure infile is (likely) an uncompressed BMP
    if (bf.signature != 0x4d42 || bi.bitCount != 24)
    {
        fclose(outptr);
        fclose(inptr);
        
        printf("Unsupported file format.\n\n");
        printf("bfType: %x, bfOffBits: %d, biSize: %d, biBitCount: %d",
             bf.signature, bf.dataOffset, bi.size, bi.bitCount);

        printf("Expected BITMAPFILEHEADER size: %li\n", sizeof(BITMAPFILEHEADER));
        printf("Input BITMAPFILEHEADER size: %u\n", bf.fileSize);
        
        // return 6;
    }
    printf("\n\nsizeof(infoheader): %lu\n\n", sizeof(bi));

    // Get image's dimensions
    int height = abs(bi.bitHeight);
    int width = abs(bi.bitWidth);

    // Allocate memory for image
    RGB(*image)[width] = calloc(height, width * sizeof(RGB));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGB)) % 4) % 4;

    printf("Reading input .bmp file...\n");
    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGB), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }
    printf("done reading.\n");


    printf("Filtering pixels of image...\n");
    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflection
        case 'r':
            reflect(height, width, image);
            break;

        // Sepia
        case 's':
            sepia(height, width, image);
            break;
    }
    printf("done filtering image pixels.\n");

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's INFOHEADER
    fwrite(&bi, sizeof(bi), 1, outptr);

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
    printf("doen writing to outfile.\n\n");

    printf("\n~~~~ BITMAPFILEHEADER ~~~~\n");

    printf("Signature: %x, File Size: %u, reserved1: %d, reserved2: %d, Offset: %d\n\n",
        bf.signature, bf.fileSize, bf.reserved1, bf.reserved2, bf.dataOffset);

    printf("\n~~~~ BITMAPCOREHEADER ~~~~\n");
    printf("No info to Display.\n");

    // Free memory for image
    free(image);

    // Close files
    fclose(inptr);
    fclose(outptr);
    return 0;
}

// Read a BITMAPV2INFOHEADER BMP
int readBMIH2(BITMAPFILEHEADER bf, BITMAPV2INFOHEADER bi, char filter, FILE *inptr, FILE *outptr)
{

    int headerSize = bf.dataOffset - (14 * sizeof(BYTE));

    // Read infile's INFOHEADER
    fread(&bi, headerSize, 1, inptr);
    

    // Ensure infile is (likely) an uncompressed BMP
    if (bf.signature != 0x4d42 || bi.bitCount != 24 || bi.compression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        
        printf("Unsupported file format.\n\n");
        printf("bfType: %x, bfOffBits: %d, biSize: %d, biBitCount: %d, biCompression: %d", bf.signature, bf.dataOffset, bi.size, bi.bitCount, bi.compression);

        printf("Expected BITMAPFILEHEADER size: %li\n", sizeof(BITMAPFILEHEADER));
        printf("Input BITMAPFILEHEADER size: %u\n", bf.fileSize);
        
        // return 6;
    }
    printf("\n\nsizeof(infoheader): %lu\n\n", sizeof(bi));

    // Get image's dimensions
    int height = abs(bi.bitHeight);
    int width = abs(bi.bitWidth);

    // Allocate memory for image
    RGB(*image)[width] = calloc(height, width * sizeof(RGB));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGB)) % 4) % 4;

    printf("Reading input .bmp file...\n");
    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGB), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }
    printf("done reading.\n");


    printf("Filtering pixels of image...\n");
    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflection
        case 'r':
            reflect(height, width, image);
            break;

        // Sepia
        case 's':
            sepia(height, width, image);
            break;
    }
    printf("done filtering image pixels.\n");

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's INFOHEADER
    fwrite(&bi, sizeof(bi), 1, outptr);

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
    printf("doen writing to outfile.\n\n");

    char* compressionType = "";
    switch (bi.compression) {
        case 0: 
            compressionType = "None"; 
            break;
        case 1: 
            compressionType = "BI_RLE8";
            break;
        case 2: 
            compressionType = "BI_RLE4";
            break;
        case 3: 
            compressionType = "BI_BITFIELDS";
            break;
        case 4: 
            compressionType = "BI_JPEG";
            break;
        case 5: 
            compressionType = "BI_PNG"; 
            break;
        case 6: 
            compressionType = "BO_ALPHABITFIELDS"; 
            break;
        case 11: 
            compressionType = "BI_CMYK";
            break;
        case 12: 
            compressionType = "BO_CMYKRLE8"; 
            break;
        case 13: 
            compressionType = "BI_CMYKRLE4"; 
            break;
        default: 
            compressionType = "Unkown";
    }

    printf("\n~~~~ BITMAPFILEHEADER ~~~~\n");

    printf("Signature: %x, File Size: %u, reserved1: %d, reserved2: %d, Offset: %d\n\n",
        bf.signature, bf.fileSize, bf.reserved1, bf.reserved2, bf.dataOffset);

    printf("\n~~~~ BITMAPV2INFOHEADER ~~~~\n");
    printf("\nCompression Bits: %x, Compression Type: %s\n", bi.compression, compressionType);

    // Free memory for image
    free(image);

    // Close files
    fclose(inptr);
    fclose(outptr);
    return 0;
    
}

// Read a BITMAPV5INFOHEADER BMP
int readBMIH5(BITMAPFILEHEADER bf, BITMAPV5INFOHEADER bi, char filter, FILE *inptr, FILE *outptr)
{

    int headerSize = bf.dataOffset - (14 * sizeof(BYTE));

    // Read infile's INFOHEADER
    fread(&bi, headerSize, 1, inptr);
    

    // Ensure infile is (likely) an uncompressed BMP
    if (bf.signature != 0x4d42 || bi.bitCount != 24 || bi.compression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        
        printf("Unsupported file format.\n\n");
        printf("bfType: %x, bfOffBits: %d, biSize: %d, biBitCount: %d, biCompression: %d", bf.signature, bf.dataOffset, bi.size, bi.bitCount, bi.compression);

        printf("Expected BITMAPFILEHEADER size: %li\n", sizeof(BITMAPFILEHEADER));
        printf("Input BITMAPFILEHEADER size: %u\n", bf.fileSize);
        
        // return 6;
    }
    printf("\n\nsizeof(infoheader): %lu\n\n", sizeof(bi));

    // Get image's dimensions
    int height = abs(bi.bitHeight);
    int width = abs(bi.bitWidth);

    // Allocate memory for image
    RGB(*image)[width] = calloc(height, width * sizeof(RGB));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGB)) % 4) % 4;

    printf("Reading input .bmp file...\n");
    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGB), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }
    printf("done reading.\n");


    printf("Filtering pixels of image...\n");
    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflection
        case 'r':
            reflect(height, width, image);
            break;

        // Sepia
        case 's':
            sepia(height, width, image);
            break;
    }
    printf("done filtering image pixels.\n");

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's INFOHEADER
    fwrite(&bi, sizeof(bi), 1, outptr);

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
    printf("doen writing to outfile.\n\n");

    char* compressionType = "";
    switch (bi.compression) {
        case 0: 
            compressionType = "None"; 
            break;
        case 1: 
            compressionType = "BI_RLE8";
            break;
        case 2: 
            compressionType = "BI_RLE4";
            break;
        case 3: 
            compressionType = "BI_BITFIELDS";
            break;
        case 4: 
            compressionType = "BI_JPEG";
            break;
        case 5: 
            compressionType = "BI_PNG"; 
            break;
        case 6: 
            compressionType = "BO_ALPHABITFIELDS"; 
            break;
        case 11: 
            compressionType = "BI_CMYK";
            break;
        case 12: 
            compressionType = "BO_CMYKRLE8"; 
            break;
        case 13: 
            compressionType = "BI_CMYKRLE4"; 
            break;
        default: 
            compressionType = "Unkown";
    }

    printf("\n~~~~ BITMAPFILEHEADER ~~~~\n");

    printf("Signature: %x, File Size: %u, reserved1: %d, reserved2: %d, Offset: %d\n\n",
        bf.signature, bf.fileSize, bf.reserved1, bf.reserved2, bf.dataOffset);

    printf("\n~~~~ BITMAPV5INFOHEADER ~~~~\n");
    printf("\nCompression Bits: %x, Compression Type: %s\n", bi.compression, compressionType);
    
    // Free memory for image
    free(image);
    
    // Close files
    fclose(inptr);
    fclose(outptr);
    return 0;

}

/*
 * Convert image file into greyscale
 */
void grayscale(int height, int width, RGB image[height][width])
{

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
 * Applies a Sepia filter to the image.
 */
void sepia(int height, int width, RGB image[height][width]) 
{

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
 * Flips the image about the y-axis.
 */
void reflect(int height, int width, RGB image[height][width]) 
{
    
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
 * Applies a blur filter to the image.
 */
void blur(int height, int width, RGB image[height][width]) 
{
    RGB(*newImage)[width] = calloc(height, width * sizeof(RGB));
    int sumRed, sumGreen, sumBlue, pels;
    int i, j;

    
    i = 0; j = 0;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            
            sumRed = 0; sumGreen = 0; sumBlue = 0; pels = 0;

            // Take 3x3 slice of pel array and average
            // the RGB values: apply to center pel.
            for (int row = -3; row <= 3; row++) {
                for (int col = -3; col <= 3; col++) {

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
