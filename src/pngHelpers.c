#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <zlib.h>

#include "pngHelpers.h"
#include "helpers.h"
#include "CRC32.h"


int filterPNG(PNGHEADER pf, PNGINFOHEADER pi,
                char filter, FILE* inptr, FILE* outptr) {

    // Allocate reasonable number of chunks
    // Use 1000 as safe upper bound to avoid massive heap allocation
    PNGCHUNK (*chunks) = calloc(MAX_CHUNKS, sizeof(PNGCHUNK));
    
    if (chunks == NULL) {
        printf("Not enough space to store png chunks.\n");
        return 23;
    }
    
    printf("Reading chunks of png... \n");
    
    // Read all chunks of the png into an array of PNGCHUNK types
    int numChunks = 0;
    size_t numIdats = 0;
    size_t dataSize = 0;
    int readingChunks = TRUE;

    while (readingChunks) {

        PNGCHUNK chunk = pngReadChunk(inptr);
        char* type = pngChunkType(chunk);

        // if (numChunks == 0) pngPrintChunk(chunk);
        
        if (!strcmp(type, "IEND")) {
            readingChunks = FALSE;
            printf("\nReached end chunk.\n");
        }

        if (!strcmp(type, "IDAT")) {
            dataSize += pngTrueLength(chunk);
            numIdats++;
        }

        // Check if we've exceeded max chunks
        if (numChunks >= MAX_CHUNKS) {
            printf("ERROR: Exceeded maximum chunk limit (%d).\n", MAX_CHUNKS);
            free(type);
            free(chunks);
            return 24;
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
    printf("Filtering pixels of image...\n");

    // Define important image reading variables
    BYTE filterType = pi.filter;
    BYTE colorType = pi.colorType;
    BYTE bitDepth = pi.bitDepth;

    // Concatenate all IDAT data fields into a byte stream
    printf("Allocating memory for IDAT data stream...\n");
    BYTE* idatStream = calloc(dataSize, sizeof(BYTE));

    if (idatStream == NULL) {
        printf("Not enough space to store data stream.\n");
        free(chunks);
        return 25;
    }
    printf("IDAT data holder memory allocated.\n");

    printf("Reading data fields only from IDAT chunks...\n");
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
    printf("Done reading IDAT data fields.\n");

    // Something is wrong if expected data amount not read
    if (index != dataSize) {
        printf("ERROR: IDAT data size mismatch.\n");
    }

    printf("Uncompressing data stream for image filtering...\n");

    long uncompressedSize = (pi.width * pngBytesPerPixel(colorType, bitDepth) + 1) * pi.height;

    RGBA* image = pngPullPixels(idatStream, dataSize,
         pi.width, pi.height, colorType, bitDepth, uncompressedSize);
         
    printf("Done uncompressing data stream.\n");
    printf("Preused uncompressed size: %ld bytes\n", uncompressedSize);

    switch (filter) {
        // Blur
        case BLUR:
            printf("Applying blur filter...\n");
            pngBlur(image, uncompressedSize);
            break;

        // Grayscale
        case GRAYSCALE:
            printf("Applying grayscale filter...\n");
            pngGrayscale(image, uncompressedSize);
            printf("Done applying grayscale filter.\n");
            break;

        // Reflect
        case REFLECT:
            printf("Applying reflect filter...\n");
            pngReflect(image, uncompressedSize);
            break;

        // Sepia
        case SEPIA:
            printf("Applying sepia filter...\n");
            pngSepia(image, uncompressedSize);
            break;

        // Red
        case REDSHIFT:
            printf("Applying red shift filter...\n");
            pngRedShift(image, uncompressedSize);
            break;

        // Green
        case GREENSHIFT:
            printf("Applying green shift filter...\n");
            pngGreenShift(image, uncompressedSize);
            break;

        // Blue
        case BLUESHIFT:
            printf("Applying blue shift filter...\n");
            pngBlueShift(image, uncompressedSize);
            break;
            
        default:
            printf("Attempted to apply unknown filter type.\n");
            break;
    }


    printf("Writing to outfile...\n");

    BYTE* imageOut = pngPushPixels(image, uncompressedSize, colorType, bitDepth, filterType);

    // Write new png to outfile
    pngEncode(pf, pi, filter, imageOut, chunks, numChunks, dataSize, outptr);
    
    printf("Done.\n");
    
    free(chunks);
    free(image);
    free(idatStream);
    free(imageOut);
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
        case 3:  // Indexed (palette)
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

RGBA* pngPullPixels(BYTE* idatStream, size_t dataSize, 
                     DWORD width, DWORD height,
                     BYTE colorType, BYTE bitDepth, long uncompressedSize) {

    
    printf("\n\nBytes per pixel: %d\n", pngBytesPerPixel(colorType, bitDepth));
    printf("Calculated uncompressed size: %ld bytes\n", uncompressedSize);
    printf("Number of total pixels: %ld\n", (uncompressedSize / pngBytesPerPixel(colorType, bitDepth)));

    RGBA* image = calloc(uncompressedSize, sizeof(BYTE));

    if (image == NULL) {
        printf("Not enough space to store image (needed %ld bytes).\n", uncompressedSize);
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
    stream.next_out = (BYTE*) image;

    inflateInit(&stream);
    int ret = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);
    
    if (ret != Z_STREAM_END) {
        printf("Inflate failed with code: %d\n", ret);
        free(image);
        return NULL;
    }
    
    return image;
}

BYTE* pngPushPixels(RGBA* image, long dataSize,
                    BYTE colorType, BYTE bitDepth, 
                    BYTE filterType) {

    BYTE* compressedData = calloc(dataSize, sizeof(BYTE));
    
    if (compressedData == NULL) {
        printf("Not enough space to store compressed data.\n");
        return NULL;
    }

    // Use zlib to deflate uncompressed data into compressed
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = dataSize;
    stream.next_in = (BYTE*) image;
    stream.avail_out = dataSize;
    stream.next_out = compressedData;
    deflateInit(&stream, Z_BEST_COMPRESSION);
    int ret = deflate(&stream, Z_FINISH);
    deflateEnd(&stream);

    if (ret != Z_STREAM_END) {
        printf("Deflate failed with code: %d\n", ret);
        free(compressedData);
        return NULL;
    }
    
    return compressedData;
}

void pngEncode(PNGHEADER pf, PNGINFOHEADER pi, BYTE filter, 
                BYTE* image, PNGCHUNK* chunks, size_t numChunks,
                long dataSize, FILE* outfile) {

    fwrite(&pf, sizeof(pf), 1, outfile);
    fwrite(&pi, sizeof(pi), 1, outfile);

    int imageIndex = 0;
    
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
            // Keep length and type the same.
            // Write new compressed data and CRC32.
            int trueLength = pngTrueLength(chunks[i]);
            memcpy(chunks[i].data, image + imageIndex, trueLength);

            chunks[i].crc = is_little_endian() ?
                reverseLong(pngCalculateCRC(chunks[i])) : pngCalculateCRC(chunks[i]);


            // Write recalculated data to outfile
            fwrite(&chunks[i].length, sizeof(DWORD), 1, outfile);
            fwrite(&chunks[i].type, sizeof(char), 4, outfile);
            fwrite(chunks[i].data, sizeof(BYTE), trueLength, outfile);
            fwrite(&chunks[i].crc, sizeof(DWORD), 1, outfile);
            imageIndex += trueLength;
        }
        free(type);

    }

}

RGBA* pngBlur(RGBA* image, long dataSize) {
    return image;
}

RGBA* pngGrayscale(RGBA* image, long dataSize) {

    for (int i = 0; i < dataSize / sizeof(RGBA); i++) {
        
        // printf("(i: %ld, ds: %ld)\n", i, dataSize);

        if (image[i].a == 0) continue;
        BYTE average = round((image[i].r + image[i].g + image[i].b) / 3);

        image[i].r = average;
        image[i].g = average;
        image[i].b = average;
    }
    return image;
}

RGBA* pngReflect(RGBA* image, long dataSize) {
    return image;
}

RGBA* pngSepia(RGBA* image, long dataSize) {
    return image;
}

RGBA* pngRedShift(RGBA* image, long dataSize) {
    return image;
}

RGBA* pngGreenShift(RGBA* image, long dataSize) {
    return image;
}

RGBA* pngBlueShift(RGBA* image, long dataSize) {
    return image;
}
