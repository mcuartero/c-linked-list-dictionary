#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <limits.h>

char *dup_string(const char *src);

void strip_newline(char *str);

unsigned int first_diff_bit(const char *left,
                            const char *right,
                            unsigned long long *bit_count);

int min(int a, int b, int c);

int editDistance(const char *s1, const char *s2, int n, int m);

#endif  // UTILS_H