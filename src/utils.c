#include "utils.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Safely duplicate a string with memory allocation */
char *dup_string(const char *src) {
    if (!src) return NULL;
    size_t len = strlen(src);
    char *copy = (char*)malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, src, len + 1);
    return copy;
}

/* Strip newline and carriage return characters from string */
void strip_newline(char *str) {
    if (!str) return;
    size_t n = strlen(str);
    while (n > 0) {
        char last = str[n - 1];
        if (last == '\n' || last == '\r') {
            n--;
            str[n] = '\0';
        } else {
            break;
        }
    }
}

static inline int msb_bit(unsigned char byte, int bit_from_msb /*0..7*/) {
    return (byte >> (7 - bit_from_msb)) & 1;
}

/* Returns first differing bit index (MSB-first within each byte).
   If strings are identical (including the NUL), returns UINT_MAX.
   If bit_count != NULL, it is incremented by the number of bits that
   actually MATCHED before the first difference. It does NOT include:
     - the differing bit itself, and
     - any bits in the NUL byte when strings are identical. */
unsigned int first_diff_bit(const char *a, const char *b, unsigned long long *bit_count) {
    assert(a && b);
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;

    unsigned long long matched_bits = 0;
    size_t i = 0;

    for (;; ++i) {
        unsigned char ab = pa[i];
        unsigned char bb = pb[i];

        if (ab == bb) {
            if (ab == 0) {
                /* Both NUL → identical. Do NOT charge the NUL byte's 8 bits. */
                if (bit_count) *bit_count += matched_bits;
                return UINT_MAX;
            }
            /* Bytes equal and non-zero → all 8 bits matched */
            matched_bits += 8ULL;
            continue;
        }

        /* Bytes differ: find first differing bit (MSB-first) */
        for (int k = 0; k < 8; ++k) {
            int abit = msb_bit(ab, k);
            int bbit = msb_bit(bb, k);
            if (abit != bbit) {
                /* Charge only the bits that matched BEFORE this differing bit */
                matched_bits += (unsigned long long)k;
                if (bit_count) *bit_count += matched_bits;
                /* Return global bit index (byte offset * 8 + bit offset from MSB) */
                return (unsigned int)(i * 8U + k);
            }
        }

        /* Should never reach here; we would have found a differing bit above. */
    }
}

/* Returns min of 3 integers
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int min(int a, int b, int c) {
    if (a < b) {
        if(a < c) {
            return a;
        } else {
            return c;
        }
    } else {
        if(b < c) {
            return b;
        } else {
            return c;
        }
    }
}

/* Returns the edit distance of two strings
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int editDistance(char *str1, char *str2, int n, int m){
    assert(m >= 0 && n >= 0 && (str1 || m == 0) && (str2 || n == 0));
    // Declare a 2D array to store the dynamic programming
    // table
    int dp[n + 1][m + 1];

    // Initialize the dp table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            // If the first string is empty, the only option
            // is to insert all characters of the second
            // string
            if (i == 0) {
                dp[i][j] = j;
            }
            // If the second string is empty, the only
            // option is to remove all characters of the
            // first string
            else if (j == 0) {
                dp[i][j] = i;
            }
            // If the last characters are the same, no
            // modification is necessary to the string.
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = min(1 + dp[i - 1][j], 1 + dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
            // If the last characters are different,
            // consider all three operations and find the
            // minimum
            else {
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
        }
    }

    // Return the result from the dynamic programming table
    return dp[n][m];
}