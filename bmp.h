#include <stdint.h>

// Redefine structs into useful names
typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint16_t WORD;

typedef struct
{
    LONG x;
    LONG y;
    LONG z;
} __attribute__((__packed__))
CIEXYZ;

// Define how a XYZTRIPLE (pixel) is structured
typedef struct
{
	CIEXYZ rgbtBlue;
	CIEXYZ rgbtGreen;
	CIEXYZ rgbtRed;
} __attribute__((__packed__))
XYZTRIPLE;

typedef struct
{
    BYTE r;
    BYTE g;
    BYTE b;
} __attribute__((__packed__))
RGB;

// Define the file header for bmp image files
typedef struct
{
	WORD signature;
	DWORD fileSize;
	WORD reserved1;
	WORD reserved2;
	DWORD dataOffset;
} __attribute__((__packed__))
BITMAPFILEHEADER;

// Define the info header for bmp imgage files
typedef struct {
    DWORD        size;
    LONG         bitWidth;
    LONG         bitHeight;
    WORD         bitPlanes;
    WORD         bitCount;
    DWORD        compression;
    DWORD        imageSize;
    LONG         xPelsPerMeter;
    LONG         yPelsPerMeter;
    DWORD        clrUsed;
    DWORD        clrImportant;
    DWORD        redMask;
    DWORD        greenMask;
    DWORD        blueMask;
    DWORD        alphaMask;
    DWORD        csType;
    XYZTRIPLE    colorPoint; // This is right
    DWORD        gammaRed;
    DWORD        gammaGreen;
    DWORD        gammeBlue;
    DWORD        intent;
    DWORD        profileData;
    DWORD        profileSize;
    DWORD        reserved3;
} BITMAPINFOHEADER;
