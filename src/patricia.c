#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "patricia.h"
#include "bit.h"     /* int getBit(char *s, unsigned int bitIndex); */
#include "utils.h"   /* editDistance(...) */
#include "search.h"  /* push_result(...), strcmp_bits_firstdiff(...) */
#include "row.h"

/* =========================
 * Internal node definition
 * ========================= */
typedef struct pnode {
    /* Internal-node fields */
    unsigned int bitIndex;       /* branching bit index (MSB-first across bytes) */
    struct pnode *left;          /* bit = 0  */
    struct pnode *right;         /* bit = 1  */

    /* Leaf fields */
    int          is_leaf;        /* 1 if this node is a leaf */
    char        *key;            /* leaf: exact key (EZI_ADD) */
    row_t      **rows;           /* leaf: dynamic array of records */
    unsigned     count;          /* leaf: number of records */
    unsigned     cap;            /* leaf: capacity of rows[] */
} pnode_t;

struct patricia_tree {
    pnode_t *root;
};

/* =========================
 * Small utilities
 * ========================= */

static char *pt_strdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char*)malloc(n);
    assert(p);
    memcpy(p, s, n);
    return p;
}

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

static pnode_t *new_internal(unsigned int bitIndex, pnode_t *a0, pnode_t *a1) {
    pnode_t *n = (pnode_t*)calloc(1, sizeof(*n));
    assert(n);
    n->bitIndex = bitIndex;
    n->left  = a0;
    n->right = a1;
    n->is_leaf = 0;
    return n;
}

static void leaf_push_row(pnode_t *leaf, row_t *row) {
    assert(leaf && leaf->is_leaf);
    if (leaf->count == leaf->cap) {
        leaf->cap = leaf->cap ? leaf->cap * 2U : 2U;
        leaf->rows = (row_t**)realloc(leaf->rows, leaf->cap * sizeof(*leaf->rows));
        assert(leaf->rows);
    }
    leaf->rows[leaf->count++] = row;
}

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

/* Push all rows of a leaf into results (preserve file order) */
static void push_leaf_records(search_stats_t *out, const pnode_t *leaf) {
    assert(leaf && leaf->is_leaf);
    for (unsigned i = 0; i < leaf->count; ++i) {
        push_result(out, leaf->rows[i]);
    }
}

/* Find first differing bit index between two C strings (MSB-first across bytes).
   If identical (including '\0'), return UINT_MAX. */
static unsigned int first_diff_bit_index(const char *a, const char *b) {
    const unsigned char *pa = (const unsigned char*)a;
    const unsigned char *pb = (const unsigned char*)b;
    unsigned int bitIndex = 0;
    for (;;) {
        unsigned char ca = *pa++;
        unsigned char cb = *pb++;
        if (ca == cb) {
            bitIndex += 8;
            if (ca == 0) return UINT_MAX; /* identical including '\0' */
            continue;
        }
        unsigned char x = (unsigned char)(ca ^ cb);
        for (int i = 7; i >= 0; --i) {
            if (x & (1u << i)) {
                return bitIndex + (unsigned)(7 - i); /* 0-based global bit index */
            }
        }
    }
}

/* Try to find the node that splits exactly at target_bit along 'key's path. */
static pnode_t *find_node_for_bit_exact(pnode_t *root, const char *key, unsigned int target_bit) {
    pnode_t *cur = root;
    while (cur && !cur->is_leaf) {
        if (cur->bitIndex == target_bit) return cur;
        int b = getBit((char*)key, cur->bitIndex);
        cur = (b == 0) ? cur->left : cur->right;
    }
    return NULL;
}

/* Find closest ancestor on key-path with bitIndex < target_bit (handles compression). */
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

/* =========================
 * Create / Free
 * ========================= */

patricia_tree_t *create_patricia_tree(void) {
    patricia_tree_t *t = (patricia_tree_t*)calloc(1, sizeof(*t));
    assert(t);
    return t;
}

void free_patricia_tree(patricia_tree_t *t) {
    if (!t) return;
    free_node(t->root);
    free(t);
}

/* =========================
 * Insert
 * ========================= */

void insert_into_patricia(patricia_tree_t *t, const char *key, row_t *row) {
    assert(t && key && row);

    if (!t->root) {
        t->root = new_leaf(key, row);
        return;
    }

    // 1) Descend to landing leaf (as you already do)
    pnode_t *cur = t->root;
    while (cur && !cur->is_leaf) {
        int b = getBit((char*)key, cur->bitIndex);
        cur = (b == 0) ? cur->left : cur->right;
    }
    if (!cur) { t->root = new_leaf(key, row); return; }

    // 2) Duplicate key? append
    if (strcmp(cur->key, key) == 0) {
        leaf_push_row(cur, row);
        return;
    }

    // 3) Find first differing bit (through '\0')
    unsigned int split = first_diff_bit_index(cur->key, key);
    if (split == UINT_MAX) {
        // strings equal by bits (shouldn't happen because strcmp != 0), but be safe
        leaf_push_row(cur, row);
        return;
    }

    // 4) Re-walk from root to find correct insertion point:
    //    Find parent s.t. parent->bitIndex < split, and next node (where) has bitIndex >= split or is a leaf.
    pnode_t *parent = NULL;
    pnode_t *where  = t->root;
    while (where && !where->is_leaf && where->bitIndex < split) {
        parent = where;
        int b = getBit((char*)key, where->bitIndex);
        where = (b == 0) ? where->left : where->right;
    }

    // 5) Create new leaf for the incoming key
    pnode_t *newleaf = new_leaf(key, row);

    // 6) Create new internal node at 'split'
    pnode_t *branch = new_internal(split, NULL, NULL);

    // Decide child sides by the NEW key’s bit at split (simplest & mirrors descent).
    if (getBit((char*)key, split) == 0) {
        branch->left  = newleaf;
        branch->right = where;
    } else {
        branch->left  = where;
        branch->right = newleaf;
    }

    // 7) Hook 'branch' into the tree
    if (!parent) {
        // Inserted above old root
        t->root = branch;
    } else {
        if (getBit((char*)key, parent->bitIndex) == 0) parent->left  = branch;
        else                                           parent->right = branch;
    }
}

/* =========================
 * DFS for closest match (no node counting)
 * ========================= */

typedef struct {
    const char *best_key;
    pnode_t    *best_leaf;
    int         best_dist;
} best_accum_t;

/* Enumerate candidates under n; DO NOT bump out->node_comparisons here.
   Choose minimum edit distance; break ties by alphabetical order. */
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

/* =========================
 * Search (descent + single compare + optional closest match)
 * ========================= */

void search_patricia(patricia_tree_t *t, const char *query,
                     search_stats_t *out, int enable_edit_distance)
{
    /* reset stats/results */
    out->results = NULL;
    out->result_count = 0;
    out->capacity = 0;
    out->bit_comparisons = 0ULL;
    out->node_comparisons = 0U;
    out->string_comparisons = 0U;

    if (!t || !t->root || !query) return;

    /* 1) Descent along branch bits — count internals + landing leaf only */
    pnode_t *parent = NULL;
    pnode_t *cur    = t->root;

    while (cur && !cur->is_leaf) {
        out->node_comparisons++;              /* count this internal */
        parent = cur;
        int b = getBit((char*)query, cur->bitIndex);
        cur = (b == 0) ? cur->left : cur->right;
    }
    if (cur) out->node_comparisons++;         /* count landing leaf */

    if (!cur) return; /* defensive */

    /* 2) Single string comparison at leaf; charge bits via helper */
    out->string_comparisons++;
    if (strcmp_bits_firstdiff(query, cur->key, &out->bit_comparisons) == 0) {
        /* exact key found — output all duplicates in file order */
        push_leaf_records(out, cur);
        return;
    }

    /* 3) Closest match (spellcheck) */
    if (!enable_edit_distance) return;

    /* Find precise mismatch bit between landing leaf and query */
    unsigned int diff = first_diff_bit_index(cur->key, query);

    /* Choose candidate subtree root:
       - prefer exact node with bitIndex == diff
       - else closest ancestor with bitIndex < diff (Patricia compression) */
    pnode_t *mismatch_node = NULL;
    if (diff != UINT_MAX) {
        mismatch_node = find_node_for_bit_exact(t->root, cur->key, diff);
        if (!mismatch_node)
            mismatch_node = find_closest_ancestor_lt_bit(t->root, cur->key, diff);
    }
    if (!mismatch_node) mismatch_node = parent ? parent : t->root;

    best_accum_t acc = (best_accum_t){0};
    dfs_best_under(mismatch_node, query, &acc, out);   /* NOTE: does NOT change n */

    if (acc.best_leaf) {
        push_leaf_records(out, acc.best_leaf);
        /* s already 1; b already charged; n unchanged during DFS by design. */
    }
}
