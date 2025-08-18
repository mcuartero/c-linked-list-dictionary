#ifndef SEARCH_H
#define SEARCH_H

#include "read.h"
#include <stddef.h>

typedef struct search_result_t {
    row_t **matches;
    size_t count;
    size_t capacity;
    size_t node_comparisons;
    size_t string_comparisons;
    size_t bit_comparisons;
} search_result_t;

search_result_t search_by_ezi_add(node_t *head, const char *key);
void free_search_result(search_result_t *result);

#endif