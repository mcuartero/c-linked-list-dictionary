#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "patricia.h"
#include "utils.h"     /* dup_string, first_diff_bit, editDistance */
#include "search.h"    /* push_result */
#include "row.h"

/* ------------------------------------------------------------------ */
/* Bit helpers (MSB-first per byte, includes the '\0' terminator)     */
/* ------------------------------------------------------------------ */
static int get_bit_msb(const unsigned char *s, unsigned int bit_index) {
    unsigned int byte = bit_index / 8U;
    unsigned int inbyte = bit_index % 8U; /* 0..7 from MSB */
    unsigned char ch = s[byte];           /* reading past strlen lands on '\0' */
    unsigned int shift = 7U - inbyte;
    return (ch >> shift) & 1U;
}

/* First differing bit (MSB-first). If identical, returns UINT_MAX.
 * Also optionally increments bit_count for the number of bits compared. */
static unsigned int first_diff_bit_msb(const char *a, const char *b,
                                       unsigned long long *bit_count) {
    /* Use your provided helper for consistency with counters. */
    return first_diff_bit(a, b, bit_count);
}

/* ------------------------------------------------------------------ */
/* Node / Tree definitions (names prefixed to avoid clash with row.h)  */
/* ------------------------------------------------------------------ */
typedef struct p_leaf_bucket {
    row_t   **rows;
    unsigned count;
    unsigned cap;
    char    *key;      /* exact key for this leaf */
} p_leaf_bucket_t;

typedef struct pnode {
    unsigned int split_bit;   /* internal node split bit (MSB index) */
    struct pnode *child[2];   /* 0-branch and 1-branch */
    p_leaf_bucket_t *leaf;    /* non-NULL means this is a leaf */
} pnode_t;

struct patricia_tree {
    pnode_t *root;
};

/* ------------------------------------------------------------------ */
/* Small utilities                                                     */
/* ------------------------------------------------------------------ */
static pnode_t *new_node_internal(unsigned int split_bit) {
    pnode_t *n = (pnode_t*)calloc(1, sizeof(*n));
    assert(n);
    n->split_bit = split_bit;
    n->child[0] = n->child[1] = NULL;
    n->leaf = NULL;
    return n;
}

static p_leaf_bucket_t *new_leaf_bucket(const char *key) {
    p_leaf_bucket_t *b = (p_leaf_bucket_t*)calloc(1, sizeof(*b));
    assert(b);
    b->key = dup_string(key);
    assert(b->key);
    return b;
}

static pnode_t *new_node_leaf(const char *key, row_t *row) {
    pnode_t *n = (pnode_t*)calloc(1, sizeof(*n));
    assert(n);
    n->split_bit = UINT_MAX; /* sentinel for leaf */
    n->leaf = new_leaf_bucket(key);
    /* push first row */
    n->leaf->cap = 2;
    n->leaf->rows = (row_t**)malloc(n->leaf->cap * sizeof(row_t*));
    assert(n->leaf->rows);
    n->leaf->rows[n->leaf->count++] = row;
    return n;
}

static void leaf_push_row(p_leaf_bucket_t *b, row_t *row) {
    if (b->count == b->cap) {
        b->cap = b->cap ? b->cap * 2 : 2;
        b->rows = (row_t**)realloc(b->rows, b->cap * sizeof(row_t*));
        assert(b->rows);
    }
    b->rows[b->count++] = row;
}

static void free_node(pnode_t *n) {
    if (!n) return;
    free_node(n->child[0]);
    free_node(n->child[1]);
    if (n->leaf) {
        free(n->leaf->key);
        free(n->leaf->rows);
        free(n->leaf);
    }
    free(n);
}

/* ------------------------------------------------------------------ */
/* Tree API                                                            */
/* ------------------------------------------------------------------ */
patricia_tree_t *create_patricia_tree(void) {
    patricia_tree_t *t = (patricia_tree_t*)calloc(1, sizeof(*t));
    assert(t);
    t->root = NULL;
    return t;
}

void free_patricia_tree(patricia_tree_t *t) {
    if (!t) return;
    free_node(t->root);
    free(t);
}

/* Insert key => row preserving duplicate order. */
void insert_into_patricia(patricia_tree_t *t, const char *key, row_t *row) {
    assert(t && key && row);

    if (!t->root) {
        t->root = new_node_leaf(key, row);
        return;
    }

    /* Walk down by bits until a leaf or missing branch. */
    pnode_t *cur = t->root;
    pnode_t *parent = NULL;
    int which_from_parent = -1;

    for (;;) {
        if (cur->leaf) {
            /* Leaf: either same key (duplicate) or we need to split. */
            unsigned int diff = first_diff_bit_msb(cur->leaf->key, key, NULL);
            if (diff == UINT_MAX) {
                /* exact same key → append row in order */
                leaf_push_row(cur->leaf, row);
                return;
            }
            /* Create an internal node at the first differing bit. */
            pnode_t *internal = new_node_internal(diff);

            int bit_old = get_bit_msb((const unsigned char*)cur->leaf->key, diff);
            int bit_new = get_bit_msb((const unsigned char*)key, diff);

            pnode_t *newleaf = new_node_leaf(key, row);

            internal->child[bit_old] = cur;
            internal->child[bit_new] = newleaf;

            /* Hook into parent or set as root */
            if (!parent) {
                t->root = internal;
            } else {
                parent->child[which_from_parent] = internal;
            }
            return;
        }

        /* Internal node: branch by split bit. */
        int b = get_bit_msb((const unsigned char*)key, cur->split_bit);
        parent = cur;
        which_from_parent = b;

        if (!cur->child[b]) {
            /* Create a fresh leaf at this branch. */
            cur->child[b] = new_node_leaf(key, row);
            return;
        }
        cur = cur->child[b];
    }
}

/* ------------------------------------------------------------------ */
/* “Closest match” collection and selection                            */
/* ------------------------------------------------------------------ */
typedef struct best_accum {
    const char *best_key;  /* do not free; points into leaf->key */
    int         best_dist;
} best_accum_t;

static void pick_best(const char *query, const char *candidate, best_accum_t *acc) {
    int d = editDistance((char*)query, (char*)candidate,
                         (int)strlen(query), (int)strlen(candidate));
    if (acc->best_key == NULL || d < acc->best_dist ||
        (d == acc->best_dist && strcmp(candidate, acc->best_key) < 0)) {
        acc->best_key = candidate;
        acc->best_dist = d;
    }
}

static void collect_best_under(pnode_t *n, const char *query, best_accum_t *acc) {
    if (!n) return;
    if (n->leaf) {
        pick_best(query, n->leaf->key, acc);
        return;
    }
    collect_best_under(n->child[0], query, acc);
    collect_best_under(n->child[1], query, acc);
}

/* After we decide the winning key, push all rows of that key into results. */
static void push_rows_of_key(pnode_t *n, const char *key, search_stats_t *out) {
    if (!n) return;
    if (n->leaf) {
        if (strcmp(n->leaf->key, key) == 0) {
            for (unsigned i = 0; i < n->leaf->count; ++i) {
                push_result(out, n->leaf->rows[i]);
            }
        }
        return;
    }
    push_rows_of_key(n->child[0], key, out);
    push_rows_of_key(n->child[1], key, out);
}

/* ------------------------------------------------------------------ */
/* Search API                                                          */
/* ------------------------------------------------------------------ */
void search_patricia(patricia_tree_t *t, const char *query,
                     search_stats_t *out, int enable_edit_distance)
{
    /* Reset stats/result buffer like your Stage 1 search. */
    out->results = NULL;
    out->result_count = 0;
    out->capacity = 0;
    out->bit_comparisons = 0ULL;
    out->node_comparisons = 0U;
    out->string_comparisons = 0U;

    if (!t || !t->root || !query) {
        return; /* NOTFOUND; caller prints */
    }

    pnode_t *cur = t->root;
    pnode_t *last_internal = NULL;

    /* Walk down the trie by bits until a leaf or missing child. */
    for (;;) {
        out->node_comparisons++;
        if (cur->leaf) {
            /* At a leaf: check exact match (count bits to first diff). */
            out->string_comparisons++;
            unsigned int diff = first_diff_bit_msb(query, cur->leaf->key, &out->bit_comparisons);
            if (diff == UINT_MAX) {
                /* Exact match → push all rows for this key. */
                for (unsigned i = 0; i < cur->leaf->count; ++i) {
                    push_result(out, cur->leaf->rows[i]);
                }
                return;
            }
            /* Not exact: break to “closest” selection below. */
            break;
        }

        last_internal = cur;
        /* One bit comparison to choose branch */
        out->bit_comparisons++;
        int b = get_bit_msb((const unsigned char*)query, cur->split_bit);

        if (!cur->child[b]) {
            /* Mismatch at this branching bit. */
            cur = NULL; /* signal mismatch; closest match under last_internal */
            break;
        }
        cur = cur->child[b];
    }

    /* If we’re here, exact not found. If edit distance disabled, we’re done. */
    if (!enable_edit_distance) {
        return; /* NOTFOUND */
    }

    /* Determine the subtree to search for closest:
     * - If we landed on a leaf (cur is leaf), the mismatch node is the parent.
     * - Else (cur==NULL), mismatch happened at last_internal. */
    pnode_t *mismatch_node = NULL;
    if (cur && cur->leaf) {
        mismatch_node = last_internal ? last_internal : t->root;
    } else {
        mismatch_node = last_internal ? last_internal : t->root;
    }

    best_accum_t acc = {0};
    collect_best_under(mismatch_node, query, &acc);
    if (!acc.best_key) {
        /* No candidates in subtree → nothing to return. */
        return;
    }

    /* Add all rows corresponding to the chosen best_key */
    push_rows_of_key(mismatch_node, acc.best_key, out);
}
