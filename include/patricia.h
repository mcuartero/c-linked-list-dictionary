#ifndef PATRICIA_H
#define PATRICIA_H

#include "row.h"
#include "search.h"

/* Opaque type */
typedef struct patricia_tree patricia_tree_t;

/* Create / destroy */
patricia_tree_t *create_patricia_tree(void);
void free_patricia_tree(patricia_tree_t *t);

/* Insert one EZI_ADD -> row mapping (preserve duplicate order) */
void insert_into_patricia(patricia_tree_t *t, const char *key, row_t *row);

/*
 * Search the Patricia trie.
 * - query: EZI_ADD typed by user (may be misspelled)
 * - out: filled with matching records and comparison counters
 * - enable_edit_distance: if non-zero, use “closest match” when exact not found
 */
void search_patricia(patricia_tree_t *t, const char *query,
                     search_stats_t *out, int enable_edit_distance);

#endif /* PATRICIA_H */
