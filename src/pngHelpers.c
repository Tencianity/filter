#include "pngHelpers.h"

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
    
    if (strcmp(pngChunkType(chunk), "IEND"))
        printf("%s, ", pngChunkType(chunk));
    else
        printf("%s\n", pngChunkType(chunk));
    
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

    int trueLength = reverseLong(header.length);
    BYTE* data = chunk.data;
    BYTE* headerData = (BYTE*) &(header.width);

    // Copy chunk data to header fields
    for (int i = 0; i < trueLength; i++) {
        headerData[i] = data[i];
    }
    header.crc = chunk.crc;

    return header;
}

BYTE pngVerifyChunk(PNGCHUNK chunk) {

    printf("Verifying chunk...\n");
    DWORD trueCRC = chunk.crc;
    DWORD trueLength = pngTrueLength(chunk);
    const static int TYPESIZE = 4;
    BYTE *data = calloc(trueLength + TYPESIZE, sizeof(BYTE));

    char *type = pngChunkType(chunk);

    // set data[0:3] to bytes of the png type field
    for (int i = 0; i < trueLength + TYPESIZE; i++) {
        if (i < TYPESIZE) {
            data[i] = type[i];
        }
        else {
            data[i] = chunk.data[i - TYPESIZE];
        }
    }

    DWORD estimateCRC = CRC32(data, trueLength + TYPESIZE);
    
    if (trueCRC != estimateCRC) {
        printf("Chunk CRC: %x\n", trueCRC);
        printf("Estimate CRC: %x\n", estimateCRC);
    }

    free(type);
    free(data);
    printf("Chunk verified!\n");
    return trueCRC == estimateCRC;
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

int filterPNG(PNGHEADER pf, PNGINFOHEADER pi,
                char filter, FILE* inptr, FILE* outptr)
{

    // Allocate reasonable number of chunks (typical PNG has 10-30 chunks)
    // Use 1000 as safe upper bound to avoid massive heap allocation
    const int MAX_CHUNKS = 1000;
    PNGCHUNK (*chunks) = calloc(MAX_CHUNKS, sizeof(PNGCHUNK));

    if (chunks == NULL) {
        printf("Not enough space to store png chunks.\n");
        return 23;
    }

    printf("Reading chunks of png... \n");
    
    // Read all chunks of the png into an array of PNGCHUNK types
    int numChunks = 0;
    int readingChunks = TRUE;

    while (readingChunks) {
        // printf("Reading chunk...\n");
        PNGCHUNK chunk = pngReadChunk(inptr);
        // printf("Chunk read!\n");
        char* type = pngChunkType(chunk);
        
        if (!strcmp(type, "IEND")) {
            readingChunks = FALSE;
            printf("\nReached end chunk.\n");
        }

        // Check if we've exceeded max chunks
        if (numChunks >= MAX_CHUNKS) {
            printf("ERROR: Exceeded maximum chunk limit (%d).\n", MAX_CHUNKS);
            free(type);
            free(chunks);
            return 24;
        }

        // if (!pngVerifyChunk(chunk)) {
        //     printf("BAD CHUNK.\n");
        //     free(type);
        //     free(chunks);
        //     return -1;
        // }

        chunks[numChunks++] = chunk;
        free(type);
    }
    printf("done!\n");

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

    }
    printf("done!\n");
    
    free(chunks);
    return 0;
}