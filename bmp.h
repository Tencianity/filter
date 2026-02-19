#include <stdint.h>

// Redefine structs into useful names
typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint16_t WORD;

typedef struct
{
    WORD x;
    WORD y;
    WORD z;
} __attribute__((__packed__))
COLORPOINT;

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
    COLORPOINT   redVector3;
    COLORPOINT   greenVector3;
    COLORPOINT   blueVector3;
    DWORD        gammaRed;
    DWORD        gammaGreen;
    DWORD        gammeBlue;
    DWORD        intent;
    DWORD        profileData;
    DWORD        profileSize;
    DWORD        reserved3;
} BITMAPINFOHEADER;

// Define how a RGBTRIPLE (pixel) is structured
typedef struct
{
	WORD rgbtBlue;
	WORD rgbtGreen;
	WORD rgbtRed;
} __attribute__((__packed__))
RGBTRIPLE;
