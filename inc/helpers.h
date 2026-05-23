#ifndef HELP_H
#define HELP_H

#include <stdint.h>
#include <string.h>
#include <getopt.h>

// Define int-type structs as identifiable names
typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint16_t WORD;

#define TRUE 1
#define FALSE 0

#define HELP 'h'
#define BLUR 'b'
#define GRAYSCALE 'g'
#define REFLECT 'r'
#define SEPIA 's'
#define REDSHIFT 'R'
#define GREENSHIFT 'G'
#define BLUESHIFT 'B'

typedef struct {
    LONG x;
    LONG y;
    LONG z;
} __attribute__((__packed__))
CIEXYZ;

// Define how a XYZTRIPLE (pixel) is structured
typedef struct {
	CIEXYZ bluePoint;
	CIEXYZ greenPoint;
	CIEXYZ redPoint;
} __attribute__((__packed__))
XYZTRIPLE;

// Define the struct for a RGB pel
typedef struct {
    BYTE b;
    BYTE g;
    BYTE r;
} __attribute__((__packed__))
RGB;

typedef struct {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
} __attribute__((__packed__))
RGBA;

static const struct option long_options[] = {
    // --blur, --grayscale, --etc...
    /* 
        {   filter_name reference by cli ("--blur"),
            no_argument/required_argument ("--blur=3"),
            flag,
            filter_name saved into getopt_long "b" or cli "-b" } 
    */
    {"blur", no_argument,           0, 'b'},
    {"grayscale", no_argument,      0, 'g'},
    {"reflect", no_argument,        0, 'r'},
    {"sepia", no_argument,          0, 's'},
    {"redshift", no_argument,       0, 'R'},
    {"greenshift", no_argument,     0, 'G'},
    {"blueshift", no_argument,      0, 'B'},
    {"help", no_argument,           0, 'h'},
    {0, 0, 0, 0}
};

char* getFileType(char* file);
DWORD reverseLong(DWORD num);
DWORD buildLong(BYTE bytes[4]);
int is_little_endian();

#endif