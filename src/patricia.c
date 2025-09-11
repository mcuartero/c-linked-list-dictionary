#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "patricia.h"
#include "bit.h"
#include "utils.h"   /* editDistance(...) */
#include "search.h"  /* search_stats_t, push_result(...) */
#include "row.h"     /* row_t */

/* ---------- Internal node & tree types ---------- */

typedef struct pnode {
    unsigned int bitIndex;      /* branching bit position (bit offset from start) */
    struct pnode *left;         /* bit = 0 */
    struct pnode *right;        /* bit = 1 */

    int           is_leaf;      /* 1 if leaf node */
    char         *key;          /* exact key string */
    row_t       **rows;         /* rows for this key */
    unsigned      count;        /* number of rows */
    unsigned      cap;          /* capacity of rows array */
} pnode_t;

struct patricia_tree {
    pnode_t *root;
};

/* ---------- Utilities & leaf-record helpers ---------- */

static char *pt_strdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char*)malloc(n);
    assert(p);
    memcpy(p, s, n);
    return p;
}

static void leaf_records_init(pnode_t *n) {
    n->rows  = NULL;
    n->count = 0U;
    n->cap   = 0U;
}

static void leaf_records_append(pnode_t *n, row_t *rec) {
    if (!n->rows) {
        n->cap = 4U;
        n->rows = (row_t**)malloc(n->cap * sizeof *n->rows);
        assert(n->rows);
    } else if (n->count == n->cap) {
        n->cap *= 2U;
        row_t **tmp = (row_t**)realloc(n->rows, n->cap * sizeof *n->rows);
        assert(tmp);
        n->rows = tmp;
    }
    n->rows[n->count++] = rec;
}

static void leaf_records_free(pnode_t *n) {
    if (!n) return;
    free(n->rows);
    n->rows  = NULL;
    n->count = n->cap = 0U;
}

/* Push all records from a leaf into search results (matches working code). */
static void results_push_leaf(search_stats_t *st, const pnode_t *leaf) {
    if (!leaf || !leaf->rows) return;
    for (unsigned int i = 0; i < leaf->count; ++i) {
        push_result(st, leaf->rows[i]);
    }
}

/* First differing BIT position between a & b, scanning through the '\0'.
   If equal strings, returns the bit position at the end (byte*8 of '\0'). */
static unsigned int first_diff_bit_pos(const char *a, const char *b) {
    unsigned int byte = 0U;
    for (;; ++byte) {
        unsigned char A = (unsigned char)a[byte];
        unsigned char B = (unsigned char)b[byte];
        if (A != B) {
            for (int bit = 7; bit >= 0; --bit) {
                int ba = (A >> bit) & 1, bb = (B >> bit) & 1;
                if (ba != bb) return byte * 8U + (unsigned)(7 - bit);
            }
        }
        if (A == '\0' && B == '\0') return byte * 8U; /* equal strings */
    }
}

/* ---------- Node creation / free ---------- */

static pnode_t *new_leaf(const char *key, row_t *row) {
    pnode_t *leaf = (pnode_t*)malloc(sizeof *leaf);
    assert(leaf);
    leaf->key = pt_strdup(key);
    leaf->bitIndex = 0U;
    leaf->is_leaf = 1;
    leaf->left = leaf->right = NULL;
    leaf_records_init(leaf);
    leaf_records_append(leaf, row);
    return leaf;
}

static pnode_t *new_internal(unsigned int bitIndex) {
    pnode_t *n = (pnode_t*)malloc(sizeof *n);
    assert(n);
    n->key = NULL;
    n->bitIndex = bitIndex;
    n->is_leaf = 0;
    n->left = n->right = NULL;
    leaf_records_init(n); /* internal nodes carry no leaf records */
    return n;
}

static void free_node(pnode_t *n) {
    if (!n) return;
    free_node(n->left);
    free_node(n->right);
    free(n->key);
    leaf_records_free(n);
    free(n);
}

/* ---------- Public API ---------- */

patricia_tree_t *create_patricia_tree(void) {
    patricia_tree_t *t = (patricia_tree_t*)malloc(sizeof *t);
    assert(t);
    t->root = NULL;
    return t;
}

void free_patricia_tree(patricia_tree_t *t) {
    if (!t) return;
    free_node(t->root);
    free(t);
}

/* Insert: descend to landing leaf; if identical key, append record.
   Otherwise split at first differing BIT (including through '\0'). */
void insert_into_patricia(patricia_tree_t *t, const char *key, row_t *row) {
    assert(t && key && row);

    if (!t->root) {
        t->root = new_leaf(key, row);
        return;
    }

    /* Descend to landing leaf using stored split bits. */
    pnode_t *node = t->root;
    while (!node->is_leaf) {
        int bit = getBit((char*)key, node->bitIndex);
        node = (bit == 0 ? node->left : node->right);
    }

    /* First differing bit between key and landing leaf key (through '\0'). */
    const char *k1 = key;
    const char *k2 = node->key;
    unsigned int lim1  = (unsigned int)((strlen(k1) + 1U) * BITS_PER_BYTE);
    unsigned int lim2  = (unsigned int)((strlen(k2) + 1U) * BITS_PER_BYTE);
    unsigned int limit = (lim1 < lim2 ? lim1 : lim2);

    unsigned int split = 0U;
    while (split < limit && getBit((char*)k1, split) == getBit((char*)k2, split)) {
        ++split;
    }

    /* Duplicate key: append to that leaf. */
    if (split == limit && strcmp(k1, k2) == 0) {
        leaf_records_append(node, row);
        return;
    }

    /* New leaf for the incoming key. */
    pnode_t *newLeaf = new_leaf(key, row);

    /* Find insertion point: deepest node with bitIndex < split. */
    pnode_t *parent = NULL;
    pnode_t *where  = t->root;
    while (!where->is_leaf && where->bitIndex < split) {
        parent = where;
        int bit = getBit((char*)key, where->bitIndex);
        where = (bit == 0 ? where->left : where->right);
    }

    /* Create split node at 'split' and attach by that bit of the new key. */
    pnode_t *branch = new_internal(split);
    if (getBit((char*)key, split) == 0) {
        branch->left  = newLeaf;
        branch->right = where;
    } else {
        branch->left  = where;
        branch->right = newLeaf;
    }

    /* Hook new internal node into the tree. */
    if (!parent) {
        t->root = branch;
    } else {
        if (getBit((char*)key, parent->bitIndex) == 0) parent->left = branch;
        else                                           parent->right = branch;
    }
}

/* DFS without touching counters: pick min edit distance, then alphabetic. */
static void dfs_best_leaf_no_count(pnode_t *node, const char *q,
                                   pnode_t **best, int *bestd) {
    if (!node) return;
    if (node->is_leaf) {
        int d = editDistance((char*)q, (char*)node->key,
                             (int)strlen(q), (int)strlen(node->key));
        if (!*best || d < *bestd || (d == *bestd && strcmp(node->key, (*best)->key) < 0)) {
            *best = node; *bestd = d;
        }
        return;
    }
    dfs_best_leaf_no_count(node->left,  q, best, bestd);
    dfs_best_leaf_no_count(node->right, q, best, bestd);
}

/* Search: matches the working file’s observable behaviour EXACTLY.
   - Initialises out with capacity = 1U.
   - Walks path counting node comparisons.
   - Does one strcmp_bits_firstdiff (updates string & bit counters).
   - If exact: pushes landing leaf’s rows.
   - Else: chooses mismatch node (exact diff bit if on path, else deepest < diff, else root),
           does DFS for "best" but (crucially) pushes the LANDING leaf’s rows if best exists. */
void search_patricia(patricia_tree_t *t, const char *query, search_stats_t *out) {
    /* Initialise like the working code */
    out->results = NULL; 
    out->result_count = 0U; 
    out->capacity = 0U;
    out->bit_comparisons = 0ULL; 
    out->node_comparisons = 0U; 
    out->string_comparisons = 0U;
    if (!t || !t->root) return;

    /* Walk while caching path (for mismatch node). */
    pnode_t *path[512]; unsigned depth = 0U;
    pnode_t *node = t->root;

    while (node && !node->is_leaf) {
        out->node_comparisons++;
        path[depth++] = node;
        int bit = getBit((char*)query, node->bitIndex);
        node = (bit == 0 ? node->left : node->right);
    }
    if (node) { out->node_comparisons++; path[depth++] = node; }

    /* One string comparison updates bit counter. */
    out->string_comparisons++;
    int cmp = strcmp_bits_firstdiff(query, node->key, &out->bit_comparisons);
    if (cmp == 0) { results_push_leaf(out, node); return; }

    /* Choose mismatch node (prefer exact bit match, else deepest < diff). */
    unsigned int diff_bit = first_diff_bit_pos(query, node->key);
    pnode_t *mismatch = NULL;
    for (int i = (int)depth - 1; i >= 0; --i) {
        pnode_t *n = path[i];
        if (!n->is_leaf && n->bitIndex == diff_bit) { mismatch = n; break; }
    }
    if (!mismatch) {
        for (int i = (int)depth - 1; i >= 0 && !mismatch; --i) {
            pnode_t *n = path[i];
            if (!n->is_leaf && n->bitIndex < diff_bit) { mismatch = n; break; }
        }
        if (!mismatch) mismatch = t->root;
    }

    /* Scan descendants (no counters) and pick best leaf. */
    pnode_t *best = NULL; int bestd = INT_MAX;
    dfs_best_leaf_no_count(mismatch, query, &best, &bestd);

    /* IMPORTANT: Mirror working code: if best exists, push the LANDING leaf (node), not best. */
    if (best) results_push_leaf(out, node);
}
