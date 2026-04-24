// https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks#CRC-32_example
#include "CRC32.h"

static uint32_t CRCTable[256];

// Initialization by multiple threads is redundant, but safe.
static void CRC32_init(void) {
    
	uint32_t crc32 = 1;
    // C guarantees CRCTable[0] = 0 already.
	for (unsigned int i = 128; i; i >>= 1) {
		crc32 = (crc32 >> 1) ^ (crc32 & 1 ? 0xedb88320 : 0);
		for (unsigned int j = 0; j < 256; j += 2*i)
        	CRCTable[i + j] = crc32 ^ CRCTable[j];
	}
}

uint32_t CRC32(const uint8_t data[], size_t data_length) {
    
	uint32_t crc32 = 0xFFFFFFFFu;

	if (CRCTable[255] == 0)
		CRC32_init();
	
	for (size_t i = 0; i < data_length; i++) {
		crc32 ^= data[i];
		crc32 = (crc32 >> 8) ^ CRCTable[crc32 & 0xFF];
	}
	
	// Finalize the CRC-32 value by inverting all the bits
	crc32 ^= 0xFFFFFFFFu;
	return crc32;
}