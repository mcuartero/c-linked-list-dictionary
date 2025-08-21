#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "read.h"
#include "search.h"

/* Grows results array using realloc */
void push_result(search_stats_t *st, row_t *rec){
    if(st->result_count == st->capacity){
        st->capacity = st->capacity ? st->capacity*2 : 2;
        st->results = realloc(st->results, st->capacity * sizeof *st->results);
        assert(st->results);
    }
    st->results[st->result_count++] = rec;
}

/* Count bits compared until first mismatch. Counts 8 bits for each equal byte, 
   for fully equal strings, includes the '\0' byte. */
int strcmp_bits_firstdiff(const char *query, const char *cur, unsigned long long *bits){
    size_t i = 0;
    /* Loops over each byte until first difference or end of string*/
    for(;;){
        unsigned char queryChar = (unsigned char)query[i];
        unsigned char curChar = (unsigned char)cur[i];
        if(queryChar == curChar){
            *bits += 8ULL;
            if(queryChar == '\0') return 0; // if at end of the string
            i++;
            continue;
        }
        /* first differing bit from bit 7 to bit 0 */
        for(int bit=7 ; bit >= 0 ; --bit){
            (*bits)++;
            int queryBit = (queryChar >> bit) & 1;
            int curBit = (curChar >> bit) & 1;
            if(queryBit != curBit) return (queryChar < curChar) ? -1 : 1;
        }
        /* unreachable (a byte must differ at some bit) */
        return (queryChar < curChar) ? -1 : 1;
    }
}

/* Performs the search lookup based on the query provided and adds the result to an array */
void search_by_ezi_add(node_t *list, const char *query, search_stats_t *out){
    /* Resetting all the search statistics */
    out->results = NULL; 
    out->result_count = 0; 
    out->capacity = 0;
    out->bit_comparisons = 0ULL;
    out->node_comparisons = 0U;
    out->string_comparisons = 0U;
    /* Loops through each node in linked list */
    for(node_t *cur = list; cur; cur = cur->next){
        out->node_comparisons++;
        out->string_comparisons++;
        if(strcmp_bits_firstdiff(query, cur->data->EZI_ADD, &out->bit_comparisons) == 0){
            push_result(out, cur->data);
        }
    }
}