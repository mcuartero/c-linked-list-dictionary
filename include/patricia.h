#ifndef PATRICIA_H
#define PATRICIA_H

#include "row.h"     /* row_t */
#include "search.h"  /* search_stats_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque Patricia tree */
typedef struct patricia_tree patricia_tree_t;

/* Create / destroy */
patricia_tree_t *create_patricia_tree(void);
void free_patricia_tree(patricia_tree_t *t);

/* Insert one record keyed by EZI_ADD; duplicates preserve file order */
void insert_into_patricia(patricia_tree_t *t, const char *key, row_t *row);

/* Search:
   enable_edit_distance != 0 -> return closest match when exact not found.
   Counters:
     - node_comparisons: nodes visited on path + enumeration DFS
     - string_comparisons: 1 (leaf compare at landing)
     - bit_comparisons: bits charged by the leaf string compare only
*/
void search_patricia(patricia_tree_t *t, const char *query,
                     search_stats_t *out, int enable_edit_distance);

#ifdef __cplusplus
}
#endif
#endif /* PATRICIA_H */
