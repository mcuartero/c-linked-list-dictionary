#include "bit.h"
#include <assert.h>

int getBit(char *s, unsigned int bitIndex) {
    assert(s);
    unsigned int byte = bitIndex / BITS_PER_BYTE;
    unsigned int indexFromLeft = bitIndex % BITS_PER_BYTE;
    unsigned int offset = (BITS_PER_BYTE - indexFromLeft - 1) % BITS_PER_BYTE;
    unsigned char byteOfInterest = s[byte];
    unsigned int offsetMask = (1 << offset);
    unsigned int maskedByte = (byteOfInterest & offsetMask);
    unsigned int bitOnly = maskedByte >> offset;
    return bitOnly;
}