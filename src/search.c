#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "read.h"
#include "search.h"

/* Grow results array and add a new result */
void push_result(search_stats_t *st, row_t *rec){
    // Check if array needs resizing
    if (st->result_count == st->capacity) {
        // Double capacity or initialize to 2 if zero
        st->capacity = st->capacity ? st->capacity * 2 : 2;
        // Reallocate memory for larger array
        st->results = realloc(st->results, st->capacity * sizeof(*st->results));
        assert(st->results);  // Ensure allocation succeeded
    }
    // Add record to results array and increment count
    st->results[st->result_count++] = rec;
}

/* Custom string comparison that counts bits compared until first mismatch */
int strcmp_bits_firstdiff(const char *query, const char *cur, unsigned long long *bits){
    size_t i = 0;
    
    // Compare bytes until difference found or strings end
    for (;;) {
        unsigned char queryChar = (unsigned char)query[i];
        unsigned char curChar = (unsigned char)cur[i];
        
        if (queryChar == curChar) {
            *bits += 8ULL;  // Add 8 bits for matching byte
            if (queryChar == '\0') return 0;  // Strings are identical
            i++;
            continue;
        }
        
        // Compare bits within the differing byte (from MSB to LSB)
        for (int bit = 7; bit >= 0; --bit) {
            (*bits)++;  // Count each bit comparison
            int queryBit = (queryChar >> bit) & 1;  // Extract query bit
            int curBit = (curChar >> bit) & 1;      // Extract current bit
            
            if (queryBit != curBit) {
                // Return comparison result at first differing bit
                return (queryChar < curChar) ? -1 : 1;
            }
        }
        
        // This point should never be reached (bytes must differ at some bit)
        return (queryChar < curChar) ? -1 : 1;
    }
}

/* Search linked list for records matching EZI_ADD field */
void search_by_ezi_add(node_t *list, const char *query, search_stats_t *out){
    // Initialize search statistics
    out->results = NULL; 
    out->result_count = 0; 
    out->capacity = 0;
    out->bit_comparisons = 0ULL;
    out->node_comparisons = 0U;
    out->string_comparisons = 0U;
    
    // Iterate through each node in linked list
    for (node_t *cur = list; cur; cur = cur->next) {
        out->node_comparisons++;        // Count node access
        out->string_comparisons++;      // Count string comparison
        
        // Compare query with EZI_ADD field, counting bits
        if (strcmp_bits_firstdiff(query, cur->data->EZI_ADD, &out->bit_comparisons) == 0) {
            push_result(out, cur->data);  // Add matching record to results
        }
    }
}