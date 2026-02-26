#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char infile[100];
    char outfile[100];
    strcpy(infile, "images/");
    strcpy(outfile, "output/");
    strcat(infile, argv[optind]);
    strcat(outfile, argv[optind + 1]);

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

    int headerSize = bf.dataOffset - (14 * sizeof(BYTE));
    char* infoheader = getInfoHeaderType(headerSize);

    switch (headerSize)
    {
        case 12:
            BITMAPCOREHEADER bc;
            readBMCH(bf, bc, filter, inptr, outptr);
            break;
        case 64:
            printf("\nInfo Header is of type %s, filter only accepts:\n", infoheader);
            printf("-\tBITMAPCOREHEADER\n");
            printf("-\tBITMAPINFOHEADER\n");
            printf("-\tBITMAPV2INFOHEADER\n");
            return 7;
        case 16:
            printf("\nInfo Header is of type %s, filter only accepts:\n", infoheader);
            printf("-\tBITMAPCOREHEADER\n");
            printf("-\tBITMAPINFOHEADER\n");
            printf("-\tBITMAPV2INFOHEADER\n");
            return 7;
        case 40:
            BITMAPINFOHEADER bi;
            readBMIH(bf, bi, filter, inptr, outptr);
            break;
        case 52:
            BITMAPV2INFOHEADER bi2;
            readBMIH2(bf, bi2, filter, inptr, outptr);
            break;
        case 56:
            printf("\nInfo Header is of type %s, filter only accepts:\n", infoheader);
            printf("-\tBITMAPCOREHEADER\n");
            printf("-\tBITMAPINFOHEADER\n");
            printf("-\tBITMAPV2INFOHEADER\n");
            return 7;
        case 108:
            printf("\nInfo Header is of type %s, filter only accepts:\n", infoheader);
            printf("-\tBITMAPCOREHEADER\n");
            printf("-\tBITMAPINFOHEADER\n");
            printf("-\tBITMAPV2INFOHEADER\n");
            return 7;
        case 124:
            BITMAPV5INFOHEADER bi5;
            readBMIH5(bf, bi5, filter, inptr, outptr);
            break;
        default: // unknown
            printf("\nInfo Header is of type %s, filter only accepts:\n", infoheader);
            printf("-\tBITMAPCOREHEADER\n");
            printf("-\tBITMAPINFOHEADER\n");
            printf("-\tBITMAPV2INFOHEADER\n");
            return 7;
    }
    


    // printf("Size: %u (%s), Bit Width: %d, Bit Height: %d, Planes: %u, bpp: %u, Compression Bits: %x, Compression Type: %s, Image Size: %u,\n Xppm: %d, Yppm: %d, Num Colors Used: %u, Important Colors: %d\n",
    //     bi.size, infoheader, bi.bitWidth, bi.bitHeight, bi.bitPlanes, 
    //     bi.bitCount, bi.compression, compressionType, 
    //     bi.imageSize, bi.xPelsPerMeter, bi.yPelsPerMeter, bi.clrUsed, bi.clrImportant);

    return 0;
}