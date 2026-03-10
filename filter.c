#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

int main(int argc, char* argv[])
{
    // Define allowable filters
    char* filters = "bgrsa";

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
        printf("Usage: ./filter [flag (-g -s -r -b)] infile outfile\n");
        return 3;
    }

    // Remember filenames
    char* infile = argv[optind];
    char* outfile = argv[optind + 1];

    // Open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 4;
    }

    // Open output file
    FILE* outptr = fopen(outfile, "w");
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

    // Make sure program reads the correct amount of bytes
    // for the info header type.
    switch (headerSize)
    {
        case 64:
            printHeaderError(infoheader);
            return 7;
        case 16:
            printHeaderError(infoheader);
            return 7;
        case 56:
            printHeaderError(infoheader);
            return 7;
        case 108:
            printHeaderError(infoheader);
            return 7;
        default:
            break;
    }

    if (infoheader == NULL)
    {
        printHeaderError(infoheader);
        return 7;
    }

    BITMAPV5INFOHEADER bi;
    readBMIH(bf, bi, filter, inptr, outptr);

    // Finish program
    return 0;
}