#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <zlib.h>

#include "pngHelpers.h"
#include "pngDecodeFilters.h"
#include "pngImageFilters.h"
#include "helpers.h"
#include "CRC32.h"

static int bytesPerPixel;

int filterPNG(PNGHEADER pf, PNGINFOHEADER pi,
                char filter[100], FILE* inptr, FILE* outptr) {

    // Allocate reasonable number of chunks
    // Use 1000 as safe upper bound to avoid massive heap allocation
    PNGCHUNK (*chunks) = calloc(MAX_CHUNKS, sizeof(PNGCHUNK));
    bytesPerPixel = pngBytesPerPixel(pi.colorType, pi.bitDepth);
    
    if (chunks == NULL) {
        printf("Not enough space to store png chunks.\n");
        return -1;
    }
    
    printf("Reading chunks of png... \t");
    
    // Read all chunks of the png into an array of PNGCHUNK types
    DWORD numChunks = 0;
    long dataSize = 0;
    int readingChunks = TRUE;

    while (readingChunks) {

        PNGCHUNK chunk = pngReadChunk(inptr);
        char* type = pngChunkType(chunk);
        
        if (!strcmp(type, "IEND")) {
            readingChunks = FALSE;
        }

        if (!strcmp(type, "IDAT")) {
            dataSize += pngTrueLength(chunk);
        }
        
        // Check if we've exceeded max chunks
        if (numChunks >= MAX_CHUNKS) {
            printf("ERROR: Exceeded maximum chunk limit (%d).\n", MAX_CHUNKS);
            free(type);
            free(chunks);
            return -1;
        }

        // Verify chunk integrity
        if (!pngVerifyChunk(chunk)) {
            printf("BAD CHUNK.\n");
            free(type);
            free(chunks);
            return -1;
        }

        chunks[numChunks++] = chunk;
        free(type);
    }

    printf("Done with (%d) chunks read.\n", numChunks);
    printf("Filtering pixels of image...\t");

    // Define important image reading variables
    BYTE colorType = pi.colorType;
    BYTE bitDepth = pi.bitDepth;

    // Concatenate all IDAT data fields into a byte stream
    BYTE* idatStream = pngGroupData(chunks, numChunks, dataSize);
    if (idatStream == NULL) {
        printf("Unable to read IDAT data from chunks.\n");
        free(chunks);
        return -1;
    }

    DWORD width = is_little_endian() ? reverseLong(pi.width) : pi.width;
    DWORD height = is_little_endian() ? reverseLong(pi.height) : pi.height;

    printf("Uncompressing data stream for image filtering...\n");
    long uncompressedSize = (width * bytesPerPixel + 1) * height;

    RGBA* image = pngPullPixels(idatStream, dataSize,
         width, height, colorType, bitDepth, pi.interlace);
    if (image == NULL) {
        printf("Unable to create pixels from image.\n");
        free(chunks);
        free(idatStream);
        return -1;
    }
    
    // Count the number of image filters being applied
    int numFilters = 0;
    while (filter[numFilters++] != -1);
    numFilters--;

    for (int i = 0; i < numFilters; i++) {
        char f = filter[i];
        switch (f) {
            // Blur
            case BLUR:
                printf("\nApplying blur filter...\n");
                pngBlur(image, width, height, bitDepth, colorType);
                break;

            // Grayscale
            case GRAYSCALE:
                printf("\nApplying grayscale filter...\n");
                pngGrayscale(image, width, height, bitDepth, colorType);
                break;

            // Reflect
            case REFLECT:
                printf("\nApplying reflect filter...\n");
                pngReflect(image, width, height, bitDepth, colorType);
                break;

            // Sepia
            case SEPIA:
                printf("\nApplying sepia filter...\n");
                pngSepia(image, width, height, bitDepth, colorType);
                break;

            // Red
            case REDSHIFT:
                printf("\nApplying red shift filter...\n");
                pngRedShift(image, width, height, bitDepth, colorType);
                break;

            // Green
            case GREENSHIFT:
                printf("\nApplying green shift filter...\n");
                pngGreenShift(image, width, height, bitDepth, colorType);
                break;

            // Blue
            case BLUESHIFT:
                printf("\nApplying blue shift filter...\n");
                pngBlueShift(image, width, height, bitDepth, colorType);
                break;
                
            default:
                printf("\nAttempted to apply unknown filter type.\n");
                printf("filter[0]: %c, filter[1]: %c, filter[2]: %c, numFilters: %d, i: %d",
                        filter[0], filter[1], filter[2], numFilters, i);
                return -1;
        }
    }


    printf("Converting new pixels to data stream...\n");
    DATASTREAM imageOut = pngPushPixels(image, uncompressedSize, width, height, colorType, bitDepth);
    if (imageOut.data == NULL) {
        free(image);
        free(chunks);
        free(idatStream);
        printf("ERROR: Unable to compress image data.\n");
        return -1;
    }
    
    
    // Write new png to outfile
    printf("Writing to outfile...\t");
    pngEncode(pf, pi, imageOut, chunks, numChunks, outptr);
    
    printf("Done.\n\n");
    
    free(chunks);
    free(image);
    free(idatStream);
    free(imageOut.data);
    return 0;
}

/* Begin support functions used in filterPNG(). */


/**
 * Reads a PNG chunk into a custom data type PNGCHUNK.
 * 
 * @param FILE* the file pointer to read the chunk from.
 * @return PNGCHUNK the chunk read from the file.
 */
PNGCHUNK pngReadChunk(FILE* pngFile) {
    
    DWORD length;
    char type[5];
    
    fread(&length, sizeof(DWORD), 1, pngFile);
    fread(&type, sizeof(char), 4, pngFile);
    type[4] = '\0';
    
    // Have to reverse the 4-byte value to get actual length
    DWORD trueLength = reverseLong(length);
    
    PNGCHUNK chunk;
    chunk.length = length;
    memcpy(chunk.type, type, 4);
    
    if (trueLength != 0) {
        chunk.data = calloc(trueLength, sizeof(BYTE));
        
        fread(chunk.data, sizeof(BYTE), trueLength, pngFile);
    } else chunk.data = NULL;
    
    fread(&chunk.crc, sizeof(DWORD), 1, pngFile);
    
    return chunk;
}

/**
 * Converts a PNGCHUNK type into a PNGINFOHEADER type
 * if it is an IHDR chunk.
 * 
 * @param PNGCHUNK The chunk to convert.
 * @return PNGINFOHEADER version of the chunk.
 */
PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK chunk) {
    
    // Get chunk->type field as a strcmp-able char array
    char* type = pngChunkType(chunk);
    PNGINFOHEADER header;

    // Make sure chunk is actually a header type
    if (strcmp(type, "IHDR")) {
        printf("%s\n", type);
        memcpy(header.type, "ERR\0", 4);
        free(type);
        return header;
    }
    free(type);
    
    // Start creating a new PNGINFOHEADER object with fields same as chunk
    header.length = chunk.length;
    memcpy(header.type, chunk.type, 4);

    int trueLength = pngTrueLength(chunk);
    BYTE* data = chunk.data;
    BYTE* headerData = (BYTE*) &(header.width);

    // Copy chunk data to header fields
    for (int i = 0; i < trueLength; i++) {
        headerData[i] = data[i];
    }
    header.crc = chunk.crc;

    return header;
}

DWORD pngCalculateCRC(PNGCHUNK chunk) {

    DWORD trueLength = pngTrueLength(chunk);
    static const int CHUNKTYPEBYTES = 4;

    char type[CHUNKTYPEBYTES + 1];
    memcpy(type, chunk.type, CHUNKTYPEBYTES);
    type[CHUNKTYPEBYTES] = '\0';
    
    BYTE* data = calloc(trueLength + CHUNKTYPEBYTES, sizeof(BYTE));

    // set data[0:3] to bytes of the png type field
    for (int i = 0; i < trueLength + CHUNKTYPEBYTES; i++) {
        if (i < CHUNKTYPEBYTES) {
            data[i] = type[i];
        }
        else {
            data[i] = chunk.data[i - CHUNKTYPEBYTES];
        }
    }

    DWORD estimateCRC = CRC32(data, trueLength + CHUNKTYPEBYTES);
    free(data);
    return estimateCRC;
}

/**
 * Verify the integrity of a PNG chunk using
 * CRC32 algorithm.
 * 
 * @param PNGCHUNK the chunk to verify.
 * @return 1 for valid chunk, 0 otherwise.
 */
BYTE pngVerifyChunk(PNGCHUNK chunk) {

    DWORD trueCRC = chunk.crc;
    DWORD estimateCRC = is_little_endian() ? 
        reverseLong(pngCalculateCRC(chunk)) : pngCalculateCRC(chunk);

    if (trueCRC != estimateCRC) {
        printf("Chunk CRC: %08x\n", trueCRC);
        printf("Estimate CRC: %08x\n", estimateCRC);
    }

    return estimateCRC == trueCRC;
}

/**
 * Finds the type of a PNGCHUNK data type.
 * 
 * @param PNGCHUNK the PNGCHUNK to find the type of.
 * @return char[5] type of the chunk with null terminator.
 */
char* pngChunkType(PNGCHUNK chunk) {
    
    char* type = calloc(5, sizeof(char));
    memcpy(type, chunk.type, 4);
    type[4] = '\0';
    return type;
}

int pngBytesPerPixel(BYTE colorType, BYTE bitDepth) {

    int bytesPerPixel;
    switch (colorType) {
        case 0:  // Grayscale
            bytesPerPixel = (bitDepth < 8) ? 1 : bitDepth / 8;
            break;
        case 2:  // RGB
            bytesPerPixel = (bitDepth / 8) * 3;
            break;
        case 3:  // Color Palette
            bytesPerPixel = 1;
            break;
        case 4:  // Grayscale + Alpha
            bytesPerPixel = (bitDepth / 8) * 2;
            break;
        case 6:  // RGBA
            bytesPerPixel = (bitDepth / 8) * 4;
            break;
        default:
            printf("Unknown color type: %d\n", colorType);
            return 0;
    }
    return bytesPerPixel;
}

/**
 * Prints the data stream of any given PNG chunk.
 * 
 * @param PNGCHUNK the chunk with data to print.
 */
void pngPrintChunk(PNGCHUNK chunk) {
    
    DWORD trueLength = pngTrueLength(chunk);
    BYTE* data = chunk.data;

    char* type = pngChunkType(chunk);
    printf("\nChunk type: %s\n", type);
    free(type);

    for (int i = 0; i < trueLength; i++) {
        printf("%#01x ", data[i]);
    }
    printf("\n");
}

RGBA* pngPullPixels(BYTE* idatStream, long dataSize, 
                     DWORD width, DWORD height,
                     BYTE colorType, BYTE bitDepth, BYTE interlace) {

    long uncompressedSize = (width * bytesPerPixel + 1) * height;

    BYTE* imageStream = calloc(uncompressedSize, sizeof(BYTE));
    if (imageStream == NULL) {
        printf("ERROR: Not enough storage to hold new image data.\n");
        return NULL;
    }
    
    // Use zlib to inflate compressed data into buffer
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    stream.avail_in = dataSize;
    stream.next_in = idatStream;
    stream.avail_out = uncompressedSize;
    stream.next_out = imageStream;
    
    inflateInit(&stream);
    int ret = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);
    
    if (ret != Z_STREAM_END) {
        printf("Inflate failed with z_code: %d\n", ret);
        free(imageStream);
        return NULL;
    }

    if (stream.total_out != uncompressedSize)
        printf("<%ld> bytes of data unaccounted for.\n", (uncompressedSize - stream.total_out));
        

    RGBA* image = pngUnfilter(imageStream, width, height);
    if (image == NULL) {
        printf("Unable to create pixels from image.\n");
        free(imageStream);
        return NULL;
    }
    
    if (interlace == 1) {
        pngUnlace(image, width, height);
    }
    
    free(imageStream);
    return image;
}

DATASTREAM pngPushPixels(RGBA* image, long dataSize,
                        DWORD width, DWORD height,
                        BYTE colorType, BYTE bitDepth) {

    BYTE* uncompressedData = calloc(dataSize, sizeof(BYTE));
    BYTE* compressedData = calloc(dataSize, sizeof(BYTE));
    BYTE* imageStream = (BYTE*) image;

    if (uncompressedData == NULL) {
        printf("Not enough space to store uncompressed data.\n");
        return (DATASTREAM){0, NULL};
    }
    
    if (compressedData == NULL) {
        printf("Not enough space to store compressed data.\n");
        return (DATASTREAM){0, NULL};
    }

    long imageByteWidth = width * bytesPerPixel;
    long dataByteWidth = width * bytesPerPixel + 1;  // +1 for filter type byte
    long imageOffset = 0;
    long dataOffset = 0;

    printf("Starting scanline filtering...\n");
    for (int i = 0; i < height; i++) {
        // {noneScore, subScore, upScore, averageScore, paethScore}
        long currImageRow = i * imageByteWidth;
        
        // Select the best byte-filter for the scanline
        FILTERTYPE bestFilter = NONE;
        int lowestScore = 999999;
        for (FILTERTYPE f = NONE; f <= PAETH; f++) {
            int newScore = pngFilterScore(imageStream + currImageRow, imageByteWidth, imageOffset, bytesPerPixel, f);
            if (newScore < lowestScore) {
                lowestScore = newScore;
                bestFilter = f;
            }
        }

        uncompressedData[dataOffset] = (BYTE) bestFilter;

        BYTE* filteredData;
        switch (bestFilter) {
            case NONE:
                memcpy(uncompressedData + dataOffset + 1, imageStream + imageOffset, imageByteWidth);
                filteredData = calloc(dataByteWidth, sizeof(BYTE));
                break;
            case SUB:
                filteredData = pngSubFilter(imageStream, imageByteWidth, imageOffset, bytesPerPixel);
                memcpy(uncompressedData + dataOffset + 1, filteredData, imageByteWidth);
                break;
            case UP:
                filteredData = pngUpFilter(imageStream, imageByteWidth, imageOffset);
                memcpy(uncompressedData + dataOffset + 1, filteredData, imageByteWidth);
                break;
            case AVERAGE:
                filteredData = pngAverageFilter(imageStream, imageByteWidth, imageOffset, bytesPerPixel);
                memcpy(uncompressedData + dataOffset + 1, filteredData, imageByteWidth);
                break;
            case PAETH:
                filteredData = pngPaethFilter(imageStream, imageByteWidth, imageOffset, bytesPerPixel);
                memcpy(uncompressedData + dataOffset + 1, filteredData, imageByteWidth);
                break;
            default:
                printf("ERROR: wtf (pngHelpers.c: 534).\n");
                free(uncompressedData);
                free(compressedData);
                return (DATASTREAM){0, NULL};
        }
        free(filteredData);
        
        imageOffset += imageByteWidth;
        dataOffset += dataByteWidth;
    }

    printf("Starting image deflating...\n");
    
    // Use zlib to deflate uncompressed data into compressed
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = dataSize;
    stream.next_in = uncompressedData;
    stream.avail_out = dataSize;
    stream.next_out = compressedData;
    deflateInit(&stream, Z_BEST_COMPRESSION);
    int ret = deflate(&stream, Z_FINISH);
    deflateEnd(&stream);

    if (ret != Z_STREAM_END) {
        printf("Deflate failed with code: %d\n", ret);
        free(uncompressedData);
        free(compressedData);
        return (DATASTREAM){0, NULL};
    }
    
    free(uncompressedData);
    DATASTREAM result = {stream.total_out, compressedData};
    return result;
}

void pngEncode(PNGHEADER pf, PNGINFOHEADER pi, DATASTREAM ds,
                PNGCHUNK* chunks, DWORD numChunks, FILE* outfile) {

    fwrite(&pf, sizeof(pf), 1, outfile);
    fwrite(&pi, sizeof(pi), 1, outfile);
    
    long datasize = ds.length;
    BYTE* data = ds.data;

    DWORD chunkSize = 8192;
    DWORD numIdatChunks = datasize / chunkSize;
    DWORD overflowBytes = datasize % chunkSize;
    if (overflowBytes != 0) numIdatChunks++;
    long imageIndex = 0;
    
    for (int i = 0; i < numChunks; i++) {

        char* type = pngChunkType(chunks[i]);
        if (strcmp(type, "IDAT")) {
            // Write non IDAT chunks without alteration
            fwrite(&chunks[i].length, sizeof(DWORD), 1, outfile);
            fwrite(&chunks[i].type, sizeof(char), 4, outfile);
            fwrite(chunks[i].data, sizeof(BYTE), pngTrueLength(chunks[i]), outfile);
            fwrite(&chunks[i].crc, sizeof(DWORD), 1, outfile);
        }

        else if (!strcmp(type, "IDAT")) {
            if (--numIdatChunks == 0 && overflowBytes != 0)
                chunkSize = overflowBytes;
            
            // Keep length and type the same.
            // Write new compressed data and CRC32.
            chunks[i].length = is_little_endian() ? 
                reverseLong(chunkSize) : chunkSize;
            
            memcpy(chunks[i].data, data + imageIndex, chunkSize);

            chunks[i].crc = is_little_endian() ?
                reverseLong(pngCalculateCRC(chunks[i])) : pngCalculateCRC(chunks[i]);


            // Write recalculated data to outfile
            fwrite(&chunks[i].length, sizeof(DWORD), 1, outfile);
            fwrite(&chunks[i].type, sizeof(char), 4, outfile);
            fwrite(chunks[i].data, sizeof(BYTE), chunkSize, outfile);
            fwrite(&chunks[i].crc, sizeof(DWORD), 1, outfile);
            imageIndex += chunkSize;
        }
        free(type);

    }
}

RGBA* pngUnfilter(BYTE* imageStream, DWORD width, DWORD height) {

    RGBA* image = calloc(width * height, sizeof(RGBA));
    if (image == NULL) {
        printf("Unable to allocate space for image pixels.\n");
        return NULL;
    }

    // Each scanline has an additional "filtertype" byte so +1
    long byteWidth = width * bytesPerPixel + 1;
    
    for (int i = 0; i < height; i++) {
        long offset = i * byteWidth;
        FILTERTYPE f = imageStream[offset];
        BYTE* unfiltered;
        int noneFilter = !f; // TRUE if f is 0 (filtertype.NONE)
        
        switch (f) {
            case NONE:
                unfiltered = imageStream + offset + 1;
                break;
            case SUB:
                unfiltered = pngUnSubFilter(imageStream, byteWidth, offset, bytesPerPixel);
                break;
            case UP:
                unfiltered = pngUnUpFilter(imageStream, byteWidth, offset);
                break;
            case AVERAGE:
                unfiltered = pngUnAverageFilter(imageStream, byteWidth, offset, bytesPerPixel);
                break;
            case PAETH:
                unfiltered = pngUnPaethFilter(imageStream, byteWidth, offset, bytesPerPixel);
                break;
            default:
                printf("Invalid chunk filter type: %d\n", f);
                return NULL;
        }

        // Simultaneously copy unfiltered data into a byte stream
        // and that data sans the filter_byte into an RGBA array.
        if (noneFilter != TRUE)
            memcpy(imageStream + offset + 1, unfiltered, byteWidth - 1);
        memcpy(image + i * width, unfiltered + 1, byteWidth - 1);

        if (noneFilter != TRUE)
            free(unfiltered);
    }

    return image;
}

RGBA* pngUnlace(RGBA* image, DWORD width, DWORD height) {
    
    const int ADAM7[8][8] = {
                        {1,6,4,6,2,6,4,6},
                        {7,7,7,7,7,7,7,7},
                        {5,6,5,6,5,6,5,6},
                        {7,7,7,7,7,7,7,7},
                        {3,6,4,6,3,6,4,6},
                        {7,7,7,7,7,7,7,7},
                        {5,6,5,6,5,6,5,6},
                        {7,7,7,7,7,7,7,7}
    };

    RGBA* unlacedImage = calloc(width * height, sizeof(RGBA));
    if (unlacedImage == NULL) {
        printf("Unable to allocate storage for image pixels.\n");
        return NULL;
    }

    long interlacedIndex = 0;
    for (int pass = 1; pass <= 7; pass++) {

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                
                int adamRow = y % 8;
                int adamCol = x % 8;

                if (ADAM7[adamRow][adamCol] == pass) {
                    long pxlIndex = x + y * width;
                    unlacedImage[interlacedIndex] = image[pxlIndex];
                    interlacedIndex++;
                }
            }
        }
    }
    
    memcpy(image, unlacedImage, width * height * sizeof(RGBA));
    free(unlacedImage);
    return image;
}

BYTE* pngGroupData(PNGCHUNK* chunks, DWORD numChunks, long dataSize) {

    BYTE* idatStream = calloc(dataSize, sizeof(BYTE));
    if (idatStream == NULL) {
        printf("Not enough space to store data stream.\n");
        return NULL;
    }

    int index = 0;
    for (int i = 0; i < numChunks; i++) {
        
        PNGCHUNK chunk = chunks[i];
        
        char* type = pngChunkType(chunk);
        if (!strcmp(type, "IDAT")) {
            DWORD trueLength = pngTrueLength(chunk);
            memcpy(idatStream + index, chunk.data, trueLength);
            index += trueLength;
        }
        free(type);
    }

    // Something is wrong if expected data amount not read
    if (index != dataSize) {
        printf("ERROR: IDAT data size mismatch.\n");
        free(idatStream);
        return NULL;
    }

    return idatStream;
}
