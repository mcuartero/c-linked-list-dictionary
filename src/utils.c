#include <stdlib.h>
#include <string.h>
#include "utils.h"

/* Safely duplicate a string with memory allocation */
char *dup_string(const char *src) {
    if (!src) return NULL;                    // Handle NULL input
    char *copy = malloc(strlen(src) + 1);     // Allocate memory for copy
    if (!copy) return NULL;                    // Check allocation success
    strcpy(copy, src);                         // Copy the string
    return copy;
}

/* Strip newline and carriage return characters from string */
void strip_newline(char *str) {
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