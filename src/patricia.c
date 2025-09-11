#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "patricia.h"
#include "bit.h"
#include "utils.h"
#include "search.h"
#include "row.h"

typedef struct pnode {
    unsigned int bitIndex;   // branching bit position
    struct pnode *left;      // bit = 0
    struct pnode *right;     // bit = 1

    int          is_leaf;    // 1 if leaf node
    char        *key;        // exact key string
    row_t      **rows;       // array of rows for this key
    unsigned     count;      // number of rows
    unsigned     cap;        // capacity of rows array
} pnode_t;

struct patricia_tree {
    pnode_t *root;
};

/* duplicate a string with malloc */
static char *pt_strdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char*)malloc(n);
    assert(p);
    memcpy(p, s, n);
    return p;
}

/* create a new leaf node for key and row */
static pnode_t *new_leaf(const char *key, row_t *row) {
    pnode_t *n = (pnode_t*)calloc(1, sizeof(*n));
    assert(n);
    n->is_leaf = 1;
    n->key = pt_strdup(key);
    n->cap = 2;
    n->rows = (row_t**)malloc(n->cap * sizeof(*n->rows));
    assert(n->rows);
    n->rows[n->count++] = row;
    return n;
}

/* create a new internal node that splits at bitIndex */
static pnode_t *new_internal(unsigned int bitIndex, pnode_t *a0, pnode_t *a1) {
    pnode_t *n = (pnode_t*)calloc(1, sizeof(*n));
    assert(n);
    n->bitIndex = bitIndex;
    n->left  = a0;
    n->right = a1;
    n->is_leaf = 0;
    return n;
}

/* add a row to an existing leaf node */
static void leaf_push_row(pnode_t *leaf, row_t *row) {
    assert(leaf && leaf->is_leaf);
    if (leaf->count == leaf->cap) {
        leaf->cap = leaf->cap ? leaf->cap * 2U : 2U;
        leaf->rows = (row_t**)realloc(leaf->rows, leaf->cap * sizeof(*leaf->rows));
        assert(leaf->rows);
    }
    leaf->rows[leaf->count++] = row;
}

/* free a node and all its children */
static void free_node(pnode_t *n) {
    if (!n) return;
    free_node(n->left);
    free_node(n->right);
    if (n->is_leaf) {
        free(n->rows);
        free(n->key);
    }
    free(n);
}

/* add all rows from a leaf into search results */
static void push_leaf_records(search_stats_t *out, const pnode_t *leaf) {
    assert(leaf && leaf->is_leaf);
    for (unsigned i = 0; i < leaf->count; ++i) {
        push_result(out, leaf->rows[i]);
    }
}

/* return first bit index where two strings differ, or UINT_MAX if same */
static unsigned int first_diff_bit_index(const char *a, const char *b) {
    const unsigned char *pa = (const unsigned char*)a;
    const unsigned char *pb = (const unsigned char*)b;
    unsigned int bitIndex = 0;
    for (;;) {
        unsigned char ca = *pa++;
        unsigned char cb = *pb++;
        if (ca == cb) {
            bitIndex += 8;
            if (ca == 0) return UINT_MAX;
            continue;
        }
        unsigned char x = (unsigned char)(ca ^ cb);
        for (int i = 7; i >= 0; --i) {
            if (x & (1u << i)) {
                return bitIndex + (unsigned)(7 - i);
            }
        }
    }
}

/* find node on path with exact bitIndex */
static pnode_t *find_node_for_bit_exact(pnode_t *root, const char *key, unsigned int target_bit) {
    pnode_t *cur = root;
    while (cur && !cur->is_leaf) {
        if (cur->bitIndex == target_bit) return cur;
        int b = getBit((char*)key, cur->bitIndex);
        cur = (b == 0) ? cur->left : cur->right;
    }
    return NULL;
}

/* find closest ancestor with bitIndex < target_bit */
static pnode_t *find_closest_ancestor_lt_bit(pnode_t *root, const char *key, unsigned int target_bit) {
    pnode_t *prev = NULL;
    pnode_t *cur  = root;
    while (cur && !cur->is_leaf) {
        if (cur->bitIndex >= target_bit) {
            return prev ? prev : root;
        }
        prev = cur;
        int b = getBit((char*)key, cur->bitIndex);
        cur = (b == 0) ? cur->left : cur->right;
    }
    return prev ? prev : root;
}

/* create a new Patricia tree */
patricia_tree_t *create_patricia_tree(void) {
    patricia_tree_t *t = (patricia_tree_t*)calloc(1, sizeof(*t));
    assert(t);
    return t;
}

/* free a Patricia tree */
void free_patricia_tree(patricia_tree_t *t) {
    if (!t) return;
    free_node(t->root);
    free(t);
}

/* insert a key/row into the Patricia tree */
void insert_into_patricia(patricia_tree_t *t, const char *key, row_t *row) {
    assert(t && key && row);

    if (!t->root) {
        t->root = new_leaf(key, row);
        return;
    }

    pnode_t *cur = t->root;
    while (cur && !cur->is_leaf) {
        int b = getBit((char*)key, cur->bitIndex);
        cur = (b == 0) ? cur->left : cur->right;
    }
    if (!cur) { t->root = new_leaf(key, row); return; }

    if (strcmp(cur->key, key) == 0) {
        leaf_push_row(cur, row);
        return;
    }

    unsigned int split = first_diff_bit_index(cur->key, key);
    if (split == UINT_MAX) {
        leaf_push_row(cur, row);
        return;
    }

    pnode_t *parent = NULL;
    pnode_t *where  = t->root;
    while (where && !where->is_leaf && where->bitIndex < split) {
        parent = where;
        int b = getBit((char*)key, where->bitIndex);
        where = (b == 0) ? where->left : where->right;
    }

    pnode_t *newleaf = new_leaf(key, row);
    pnode_t *branch  = new_internal(split, NULL, NULL);

    if (getBit((char*)key, split) == 0) {
        branch->left  = newleaf;
        branch->right = where;
    } else {
        branch->left  = where;
        branch->right = newleaf;
    }

    if (!parent) {
        t->root = branch;
    } else {
        if (getBit((char*)key, parent->bitIndex) == 0) parent->left  = branch;
        else                                           parent->right = branch;
    }
}

typedef struct {
    const char *best_key;
    pnode_t    *best_leaf;
    int         best_dist;
} best_accum_t;

/* depth-first search under node to find closest key by edit distance */
static void dfs_best_under(pnode_t *n, const char *query,
                           best_accum_t *acc, search_stats_t *out)
{
    if (!n) return;

    if (n->is_leaf) {
        int d = editDistance((char*)query, (char*)n->key,
                             (int)strlen(query), (int)strlen(n->key));
        if (acc->best_leaf == NULL ||
            d < acc->best_dist ||
            (d == acc->best_dist && strcmp(n->key, acc->best_key) < 0)) {
            acc->best_leaf = n;
            acc->best_key  = n->key;
            acc->best_dist = d;
        }
        return;
    }
    dfs_best_under(n->left,  query, acc, out);
    dfs_best_under(n->right, query, acc, out);
}

/* search the Patricia tree for a query string */
void search_patricia(patricia_tree_t *t, const char *query,
                     search_stats_t *out, int enable_edit_distance) {
    out->results = NULL;
    out->result_count = 0;
    out->capacity = 0;
    out->bit_comparisons = 0ULL;
    out->node_comparisons = 0U;
    out->string_comparisons = 0U;

    if (!t || !t->root || !query) return;

    pnode_t *parent = NULL;
    pnode_t *cur    = t->root;

    while (cur && !cur->is_leaf) {
        out->node_comparisons++;
        parent = cur;
        int b = getBit((char*)query, cur->bitIndex);
        cur = (b == 0) ? cur->left : cur->right;
    }
    if (cur) out->node_comparisons++;

    if (!cur) return;

    out->string_comparisons++;
    if (strcmp_bits_firstdiff(query, cur->key, &out->bit_comparisons) == 0) {
        push_leaf_records(out, cur);
        return;
    }

    if (!enable_edit_distance) return;

    unsigned int diff = first_diff_bit_index(cur->key, query);

    pnode_t *mismatch_node = NULL;
    if (diff != UINT_MAX) {
        mismatch_node = find_node_for_bit_exact(t->root, cur->key, diff);
        if (!mismatch_node)
            mismatch_node = find_closest_ancestor_lt_bit(t->root, cur->key, diff);
    }
    if (!mismatch_node) mismatch_node = parent ? parent : t->root;

    best_accum_t acc = (best_accum_t){0};
    dfs_best_under(mismatch_node, query, &acc, out);

    if (acc.best_leaf) {
        push_leaf_records(out, acc.best_leaf);
    }
}