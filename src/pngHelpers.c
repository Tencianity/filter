#include "pngHelpers.h"

PNGCHUNK pngReadChunk(FILE* pngFile) {
    DWORD length;
    char type[5];
    BYTE* data;
    DWORD crc;
    
    fread(&length, sizeof(DWORD), 1, pngFile);
    fread(&type, sizeof(char), 4, pngFile);
    type[4] = '\0';
    
    // Have to reverse the 4-byte value to get actual length
    DWORD trueLength = reverseLong(length);
    data = calloc(trueLength, sizeof(BYTE));

    fread(data, sizeof(BYTE), trueLength, pngFile);
    fread(&crc, sizeof(DWORD), 1, pngFile);

    PNGCHUNK chunk;
    chunk.length = length;
    strcpy(chunk.type, type);
    chunk.data = data;
    chunk.crc = crc;
    
    return chunk;
}

PNGINFOHEADER pngConvertChunkToHeader(PNGCHUNK chunk) {
    // Get chunk->type field as a strcmp-able char array
    char* type = pngChunkType(chunk);

    PNGINFOHEADER header;

    // Make sure chunk is actually a header type
    if (strcmp(type, "IHDR")) {
        printf("%s\n", type);
        strcpy(header.type, (char[4]) {'E', 'R', 'R', '\0'});
        free(type);
        return header;
    }
    free(type);
    
    // Start creating a new PNGINFOHEADER object with fields same as chunk
    header.length = chunk.length;
    strcpy(header.type, chunk.type);

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

/**
 * Finds the type of a PNGCHUNK with and ending '\0' char
 */
char* pngChunkType(PNGCHUNK chunk) {
    char* type = calloc(5, sizeof(char));
    strcpy(type, chunk.type);
    type[4] = '\0';
    return type;
}

int filterPNG(PNGHEADER pf, PNGINFOHEADER pi,
                char filter, FILE* inptr, FILE* outptr)
{

    DWORD width = pi.width;
    DWORD height = pi.height;
    long sizeOfChunks = (width * height) - (sizeof(pf) + sizeof(pi));
    PNGCHUNK (*chunks) = calloc(width * height, sizeof(PNGCHUNK));

    if (chunks == NULL) {
        printf("Not enough space to store png chunks.\n");
        return 23;
    }

    printf("Reading chunks of png... \n");
    
    // Read all chunks of the png into an array of PNGCHUNK types
    int numChunks = 0;
    int readingChunks = TRUE;
    while (readingChunks) {
        PNGCHUNK chunk = pngReadChunk(inptr);
        
        if (!strcmp(pngChunkType(chunk), "IEND")) {
            readingChunks = FALSE;
            printf("\nReached end chunk.\n");
        }

        chunks[numChunks++] = chunk;
    }
    printf("done!\n");

    // Write File Header and Info Header to outfile
    fwrite(&pf, sizeof(pf), 1, outptr);
    fwrite(&pi, sizeof(pi), 1, outptr);

    printf("Writing to outfile...\n");
    
    // Write rest of chunk data from array to outfile
    for (int i = 0; i < numChunks; i++) {
        PNGCHUNK* chunk = &chunks[i];

        fwrite(chunk, pngChunkSize(*chunk), 1, outptr);
    }
    printf("done!\n");
    
    free(chunks);
    return 0;
}