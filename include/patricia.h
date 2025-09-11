#ifndef PATRICIA_H
#define PATRICIA_H

#include "row.h"
#include "search.h"

/* Opaque tree type */
typedef struct patricia_tree patricia_tree_t;

/* Create an empty Patricia tree. Caller frees with free_patricia_tree(). */
patricia_tree_t *create_patricia_tree(void);

/* Free the entire Patricia tree (all nodes & rows array pointers). */
void free_patricia_tree(patricia_tree_t *t);

/* Insert a (key,row) pair into the tree. If key already present, appends row. */
void insert_into_patricia(patricia_tree_t *t, const char *key, row_t *row);

/* Search for a key. 
   - If exact match, all matching rows are pushed into out.
   - Otherwise, finds the “closest” key by edit distance (Levenshtein).
   - Counters in out are updated (bit/node/string comparisons).
   - enable_edit_distance can be 0 to disable fuzzy search (just stops at exact leaf). */
void search_patricia(patricia_tree_t *t, const char *query,
                     search_stats_t *out);

#endif /* PATRICIA_H */
