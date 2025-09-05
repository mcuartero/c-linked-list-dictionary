#ifndef PRINT_H
#define PRINT_H
#include <stdio.h>
#include "row.h"

void strip_newline(char *str);
void print_record(FILE *out, const row_t *a);

#endif // PRINT_H