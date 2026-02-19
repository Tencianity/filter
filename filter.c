#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    // Define allowable filters
    char *filters = "bgrs";

    // Get filter flag and check validity
    char filter = getopt(argc, argv, filters);
    if (filter == '?')
    {
        printf("Invalid filter.\n");
        return 1;
    }

    // Ensure only one filter
    if (getopt(argc, argv, filters) != -1)
    {
        printf("Only one filter allowed.\n");
        return 2;
    }

    // Ensure proper usage
    if (argc != optind + 2)
    {
        printf("Usage: ./filter [flag] infile outfile\n");
        return 3;
    }

    // Remember filenames
    char *infile = argv[optind];
    char *outfile = argv[optind + 1];

    // Open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 4;
    }

    // Open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 5;
    }

    // Read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Ensure infile is (likely) a uncompressed BMP 5.0
    if (bf.signature != 0x4d42 || bf.dataOffset != 138 || bi.size != 124 ||
        bi.bitCount != 24 || bi.compression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        
        printf("Unsupported file format.\n\n");
        printf("bfType: %x, bfOffBits: %d, biSize: %d, biBitCount: %d, biCompression: %d", bf.signature, bf.dataOffset, bi.size, bi.bitCount, bi.compression);

        printf("Expected BITMAPFILEOHEADER size: %li\n", sizeof(BITMAPFILEHEADER));
        printf("Input BITMAPFILEHEADER size: %u\n", bf.fileSize);
        
        return 6;
    }

    // Get image's dimensions
    int height = abs(bi.bitHeight);
    int width = abs(bi.bitWidth);

    // Allocate memory for image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    printf("Reading input .bmp file...\n");
    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }
    printf("done reading.\n");

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

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

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

    printf("\n~~~~ BITMAPINFOHEADER ~~~~\n");

    printf("Size: %u, Bit Width: %d, Bit Height: %d, Planes: %u, bpp: %u, Compression Bits: %x, Compression Type: %s, Image Size: %u,\n Xppm: %d, Yppm: %d, Num Colors Used: %u, Important Colors: %d\n",
        bi.size, bi.bitWidth, bi.bitHeight, bi.bitPlanes, 
        bi.bitCount, bi.compression, compressionType, 
        bi.imageSize, bi.xPelsPerMeter, bi.yPelsPerMeter, bi.clrUsed, bi.clrImportant);
    
    printf("Masks (RGBa): %u %u %u %u\n", bi.redMask, bi.greenMask, bi.blueMask, bi.alphaMask);
    printf("CS Type: %u\n", bi.csType);
    printf("Color Points (RGB): (%u, %u, %u), (%u, %u, %u), (%u, %u, %u)\n\n",
        bi.redVector3.x, bi.redVector3.y, bi.redVector3.z,
        bi.greenVector3.x, bi.greenVector3.y, bi.greenVector3.z,
        bi.blueVector3.x, bi.blueVector3.y, bi.blueVector3.z);
    // Free memory for image
    free(image);

    // Close files
    fclose(inptr);
    fclose(outptr);
    return 0;
}