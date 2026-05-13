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
    BYTE* idatStream = calloc(dataSize, sizeof(BYTE));
    int index = 0;
    for (int i = 0; i < numChunks; i++) {
        
        PNGCHUNK chunk = chunks[i];
        
        if (!strcmp(pngChunkType(chunk), "IDAT")) {
            DWORD trueLength = pngTrueLength(chunk);
            memcpy(idatStream + index, chunk.data, trueLength);
            index += trueLength;
        }
    }

    // Something is wrong if expected data amount not read
    if (index != dataSize) {
        printf("ERROR: IDAT data size mismatch.\n");
    }

    RGBA* image = pngPullPixels(idatStream, dataSize, colorType, bitDepth);

    switch (filter) {
        // Blur
        case BLUR:
            pngBlur(image, dataSize, filterType, colorType, bitDepth);
            break;

        // Grayscale
        case GRAYSCALE:
            pngGrayscale(image, dataSize, filterType, colorType, bitDepth);
            break;

        // Reflect
        case REFLECT:
            pngReflect(image, dataSize, filterType, colorType, bitDepth);
            break;

        // Sepia
        case SEPIA:
            pngSepia(image, dataSize, filterType, colorType, bitDepth);
            break;

        // Red
        case REDSHIFT:
            pngRedShift(image, dataSize, filterType, colorType, bitDepth);
            break;

        // Green
        case GREENSHIFT:
            pngGreenShift(image, dataSize, filterType, colorType, bitDepth);
            break;

        // Blue
        case BLUESHIFT:
            pngBlueShift(image, dataSize, filterType, colorType, bitDepth);
            break;
    }

    printf("Writing to outfile...\n");

    // Write File Header and Info Header to outfile
    fwrite(&pf, sizeof(pf), 1, outptr);
    fwrite(&pi, sizeof(pi), 1, outptr);
    
    // Write rest of chunk data from array to outfile
    for (int i = 0; i < numChunks; i++) {
        
        PNGCHUNK chunk = chunks[i];
        DWORD trueLength = pngTrueLength(chunk);

        // Write each chunk field to outfile
        fwrite(&chunk.length, sizeof(DWORD), 1, outptr);
        fwrite(&chunk.type, sizeof(char), 4, outptr);
        if (trueLength > 0) {
            fwrite(chunk.data, sizeof(BYTE), trueLength, outptr);
        }
        fwrite(&chunk.crc, sizeof(DWORD), 1, outptr);

        // Print to console if any bad chunks get written to outfile
        if (!pngVerifyChunk(chunk)) {
            printf("Bad chunk written to outfile: Chunk-%d\n", i + 1);
        }
    }
    printf("Done.\n");
    
    free(chunks);
    return 0;
}

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

/*
    if (strcmp(pngChunkType(chunk), "IEND"))
        printf("%s, ", pngChunkType(chunk));
    else
        printf("%s\n", pngChunkType(chunk));
*/
    
    return chunk;
}

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

    char type[5];
    memcpy(type, chunk.type, 4);
    type[4] = '\0';
    
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

/**
 * Prints the data stream of any given PNG chunk.
 * 
 * @param PNGCHUNK the chunk with data to print.
 */
void pngPrintChunk(PNGCHUNK chunk) {
    
    DWORD trueLength = pngTrueLength(chunk);
    BYTE* data = chunk.data;

    printf("\nChunk type: %s\n", pngChunkType(chunk));
    for (int i = 0; i < trueLength; i++) {
        printf("%#01x ", data[i]);
    }
    printf("\n");
}

RGBA* pngPullPixels(BYTE* idatStream, size_t dataSize, BYTE colorType, BYTE bitDepth) {
    
    // Theoretical max compression of data with zlib is 1:1032 bytes
    const static int MAXCOMPRESSION = 1032;
    RGBA* image = calloc(dataSize * MAXCOMPRESSION, sizeof(RGBA));

    // Use zlib to deflate compressed data into RGBA array
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    stream.avail_in = dataSize;
    stream.next_in = idatStream;
    stream.next_out = (BYTE*) image;

    inflateInit(&stream);
    inflate(&stream, Z_NO_FLUSH);
    inflateEnd(&stream);
    
    return image;
}

RGBA* pngBlur(RGBA* image, size_t dataSize, BYTE filterType, BYTE colorType, BYTE bitDepth) {
    return image;
}

RGBA* pngGrayscale(RGBA* image, size_t dataSize, BYTE filterType, BYTE colorType, BYTE bitDepth) {

    for (int i = 0; i < dataSize / 4; i++) {
        if (image[i].a == 0) continue;
        BYTE average = round((image[i].r + image[i].g + image[i].b) / 3);

        image[i].r = average;
        image[i].g = average;
        image[i].b = average;
    }
    return image;
}

RGBA* pngReflect(RGBA* image, size_t dataSize, BYTE filterType, BYTE colorType, BYTE bitDepth) {
    return image;
}

RGBA* pngSepia(RGBA* image, size_t dataSize, BYTE filterType, BYTE colorType, BYTE bitDepth) {
    return image;
}

RGBA* pngRedShift(RGBA* image, size_t dataSize, BYTE filterType, BYTE colorType, BYTE bitDepth) {
    return image;
}

RGBA* pngGreenShift(RGBA* image, size_t dataSize, BYTE filterType, BYTE colorType, BYTE bitDepth) {
    return image;
}

RGBA* pngBlueShift(RGBA* image, size_t dataSize, BYTE filterType, BYTE colorType, BYTE bitDepth) {
    return image;
}
