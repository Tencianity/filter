#include "filter.h"

int main(int argc, char* argv[]) {
    
    /* Implemented filters:
        --blur: blur
        --grayscale: grayscale
        --reflection: reflection
        --sepia: sepia
        --redshift: red shift
        --greenshift: green shift
        --blueshift: blue shift
    */

    char* filters = "bgrsRGBh";
    char filter;

    // Get filter flag and check it is a valid option
    int option_index = 0;
    if ((filter = getopt_long(argc, argv, filters, long_options, &option_index)) == -1) {
        printf("Unknown option. Use --help for usage information.\n");
        return 1;
    }

    // Ensure only one filter
    if (getopt_long(argc, argv, filters, long_options, NULL) != -1) {
        printf("Only one filter allowed.\n");
        return 2;
    }

    // Display help message
    if (filter == HELP) {   
        printf("Usage: ./filter [OPTIONS] infile outfile\n\n");
        printf("OPTIONS:\n"
               "  --blur          Blurs the image.\n"
               "  --grayscale     Converts the image to grayscale.\n"
               "  --reflect       Reflects the image horizontally.\n"
               "  --sepia         Applies a sepia filter to the image.\n"
               "  --redshift      Increases the red values of the image.\n"
               "  --greenshift    Increases the green values of the image.\n"
               "  --blueshift     Increases the blue values of the image.\n");
        return 0;
    }

    // Ensure proper usage
    if (argc != optind + 2) {
        printf("Usage: ./filter [flag (--blueshift --grayscale --help)] infile outfile\n");
        return 3;
    }


    // Remember filenames
    char* infile = argv[optind];
    char* outfile = argv[optind + 1];

    // Open input file
    FILE* inptr = fopen(infile, "rb");

    if (inptr == NULL) {
        printf("Could not open %s.\n", infile);
        return 4;
    }

    // Open output file
    FILE* outptr = fopen(outfile, "wb+");

    if (outptr == NULL) {
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
        switch (headerSize) {
            case 64:
            case 16:
            case 56:
            case 108:
                printHeaderError(infoheader);
                fclose(inptr);
                fclose(outptr);
                return 8;
            default:
                break;
        }

        if (infoheader == NULL) {
            printHeaderError(infoheader);
            fclose(inptr);
            fclose(outptr);
            return 9;
        }

        BITMAPV5INFOHEADER bi;
        filterBMP(bf, bi, filter, inptr, outptr);
    }
    else if (!strcmp(getFileType(infile), "png")) {
        PNGHEADER pf;
        fread(&pf, sizeof(PNGHEADER), 1, inptr);
        PNGINFOHEADER pi;
        fread(&pi, sizeof(PNGINFOHEADER), 1, inptr);

        if (!strcmp(pi.type, "ERR")) return 22;

        DWORD width = is_little_endian() ? reverseLong(pi.width) : pi.width;
        DWORD height = is_little_endian() ? reverseLong(pi.height) : pi.height;
        DWORD crc = is_little_endian() ? reverseLong(pi.crc) : pi.crc;
        
        printf( "FILE BITS: %c%c%c\n"
                "highBit: %#.2x\ndos: %#.4x\neof: %#.2x\nline ending: %#.2x\n",
                pf.png[0], pf.png[1], pf.png[2],
                pf.highBit, pf.dos, pf.endOfFile, pf.lineEnding
        );
        printf("\n");


        printf( "Length: %#.8x\n"
                "Type: %c%c%c%c\n",
                reverseLong(pi.length), pi.type[0], pi.type[1], pi.type[2], pi.type[3]
        );
        printf( "width: %u\theight: %u\tbitdepth: %u\ncolor type: %d\t"
                "compression: %d\tfilter type: %d\ninterlace: %d\tCRC: %x.\n",
                width, height, pi.bitDepth,
                pi.colorType, pi.compression,
                pi.filter, pi.interlace, crc
        );
        printf("\n");
        filterPNG(pf, pi, filter, inptr, outptr);
    }


    // Finish program
    fclose(inptr);
    fclose(outptr);
    return 0;
}