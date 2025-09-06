#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "row.h"

/* Search result + counters */
typedef struct search_stats {
    row_t **results;          /* dynamic array of matches (pointers) */
    unsigned int result_count;
    unsigned int capacity;
    unsigned long long bit_comparisons;   /* bit accesses */
    unsigned int node_comparisons;  /* nodes visited */
    unsigned int string_comparisons;/* string comps performed */
} search_stats_t;

/* Bit comparisions until first difference */
int strcmp_bits_firstdiff(const char *a, const char *b, unsigned long long *bits);

/* Grows results array dynamically and adds results to array */
void push_result(search_stats_t *st, row_t *rec);

/* Performs search by EZI_ADD and fills search_stats */
void search_by_ezi_add(node_t *list, const char *query, search_stats_t *out);

#endif