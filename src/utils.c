#include "utils.h"
#include <stdio.h>
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

unsigned int first_diff_bit(const char *left,
                            const char *right,
                            unsigned long long *bit_count)
{
    // Handle NULL pointers
    if (!left || !right) {
        return 0;
    }
    
    size_t byte_index = 0;

    for (;;) {
        unsigned char lb = (unsigned char)left[byte_index];
        unsigned char rb = (unsigned char)right[byte_index];

        if (lb == rb) {
            if (bit_count) *bit_count += 8ULL;
            if (lb == 0) {
                /* both ended → identical */
                return UINT_MAX;
            }
            byte_index++;
            continue;
        }

        for (int bit = 7; bit >= 0; --bit) {
            if (bit_count) (*bit_count)++;
            int bl = (lb >> bit) & 1;
            int br = (rb >> bit) & 1;
            if (bl != br) {
                unsigned int bit_in_byte = (unsigned int)(7 - bit);
                return (unsigned int)(byte_index * 8 + bit_in_byte);
            }
        }

        return (unsigned int)(byte_index * 8);
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