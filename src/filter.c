#include "filter.h"

int main(int argc, char* argv[])
{
    // Define allowable filters
    char* filters = "bgrsxyz";

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
        printf("Usage: ./filter [flag (-x -y -z -g -s -r -b)] infile outfile\n");
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

    // Error if infile type not same as out file type
    if (strcmp(getFileType(infile), getFileType(outfile))) {
        printf(
            "Infile type: %s\nOutfile type: %s\n\n Input and output files must be of same type.\n",
            getFileType(infile), getFileType(outfile)
        );

        fclose(inptr);
        fclose(outptr);
        return 7;
    }

    // Read the filetype (bmp or png for now)
    printf("\nInput file type: %s\n\n", getFileType(infile));

    // Condition to filter bmp files
    if (!strcmp(getFileType(infile), "bmp")) {

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
                fclose(inptr);
                fclose(outptr);
                return 7;
            case 16:
                printHeaderError(infoheader);
                fclose(inptr);
                fclose(outptr);
                return 7;
            case 56:
                printHeaderError(infoheader);
                fclose(inptr);
                fclose(outptr);
                return 7;
            case 108:
                printHeaderError(infoheader);
                fclose(inptr);
                fclose(outptr);
                return 7;
            default:
                break;
        }

        if (infoheader == NULL)
        {
            printHeaderError(infoheader);
            fclose(inptr);
            fclose(outptr);
            return 7;
        }

        BITMAPV5INFOHEADER bi;
        filterBMP(bf, bi, filter, inptr, outptr);
    }
    else if (!strcmp(getFileType(infile), "png")) {
        PNGHEADER pf;
        fread(&pf, sizeof(PNGHEADER), 1, inptr);
        PNGCHUNK headerChunk = pngReadChunk(inptr);
        PNGINFOHEADER pi = pngConvertChunkToHeader(headerChunk);

        if (!strcmp(pi.type, "ERR")) return 22;
        
        printf( "FILE BITS: %c%c%c\n"
                "highBit: %#x\ndos: %#x\neof: %#x\nline ending: %#x\n",
                pf.png[0], pf.png[1], pf.png[2],
                pf.highBit, pf.dos, pf.endOfFile, pf.lineEnding
        );
        printf("\n");


        printf( "Length: %#x\n"
                "Type: %c%c%c%c\n",
                reverseLong(pi.length), pi.type[0], pi.type[1], pi.type[2], pi.type[3]
        );
        printf( "width: %u\nheight: %u\nbitdepth: %u\ncolor type: %d\n"
                "compression: %d\nfilter type: %d\ninterlace: %d\nCRC: %x.\n",
                pi.width, pi.height, pi.bitDepth,
                pi.colorType, pi.compression,
                pi.filter, pi.interlace, pi.crc
        );
        printf("\n");
        filterPNG(pf, pi, filter, inptr, outptr);
    }


    // Finish program
    fclose(inptr);
    fclose(outptr);
    return 0;
}