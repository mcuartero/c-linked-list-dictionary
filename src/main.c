#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "read.h"
#include "list.h"
#include "search.h"
#include "print.h"
#include "utils.h"

#ifdef ENABLE_PATRICIA
#include "patricia.h"
#endif

static void usage(const char *prog){
    fprintf(stderr, "Usage: %s <stage> <input.csv> <output.txt>\n", prog);
    exit(1);
}

#ifndef ENABLE_PATRICIA
#else
static void print_results(FILE *out, const search_stats_t *st) {
    if (!st || st->result_count == 0) {
        fprintf(out, "NOTFOUND\n");
        return;
    }
    for (unsigned i = 0; i < st->result_count; ++i) {
        print_record(out, st->results[i]);
    }
}
#endif

static void run_stage1(node_t *list, FILE *fout) {
    char q[1024];
    while (fgets(q, sizeof(q), stdin)) {
        strip_newline(q);

        search_stats_t st;
        search_by_ezi_add(list, q, &st);

        /* file output */
        fprintf(fout, "%s\n", q);
        if (st.result_count == 0) {
            fprintf(fout, "NOTFOUND\n");
        } else {
            for (unsigned int i = 0; i < st.result_count; i++) {
                print_record(fout, st.results[i]);
            }
        }

        /* stdout summary */
        printf("%s --> %u records found - comparisons: b%llu n%u s%u\n",
               q, st.result_count,
               (unsigned long long)st.bit_comparisons,
               st.node_comparisons, st.string_comparisons);

        free(st.results);
    }
}

#ifdef ENABLE_PATRICIA

static void run_stage2(node_t *list, FILE *fout) {
    patricia_tree_t *tree = create_patricia_tree();
    if (!tree) {
        fprintf(stderr, "Error: could not create Patricia tree\n");
        return;
    }

    /* build tree from linked list (preserve order for duplicates) */
    for (node_t *cur = list; cur; cur = cur->next) {
        if (cur->data && cur->data->EZI_ADD) {
            insert_into_patricia(tree, cur->data->EZI_ADD, cur->data);
        }
    }

    char q[1024];
    while (fgets(q, sizeof(q), stdin)) {
        strip_newline(q);

        search_stats_t st;
        /* enable_edit_distance = 1 */
        search_patricia(tree, q, &st, 1);

        /* file output */
        fprintf(fout, "%s\n", q);
        print_results(fout, &st);

        /* stdout summary */
        printf("%s --> %u records found - comparisons: b%llu n%u s%u\n",
               q, st.result_count,
               (unsigned long long)st.bit_comparisons,
               st.node_comparisons, st.string_comparisons);

        free(st.results);
    }

    free_patricia_tree(tree);
}
#endif

int main(int argc, char *argv[]) {
    if (argc != 4) usage(argv[0]);

#ifndef ENABLE_PATRICIA
    if (strcmp(argv[1], "1") != 0) {
        fprintf(stderr, "This build excludes Patricia (stage 2). Use dict2.\n");
        usage(argv[0]);
    }
#else
    if (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0) {
        usage(argv[0]);
    }
#endif

    const char *input_csv  = argv[2];
    const char *output_txt = argv[3];

    node_t *list = read_csv(input_csv);
    if (!list) {
        fprintf(stderr, "Error: failed to read CSV or file is empty\n");
        return 1;
    }

    FILE *fout = fopen(output_txt, "w");
    if (!fout) {
        perror("open output");
        free_list(list);
        return 1;
    }

#ifndef ENABLE_PATRICIA
    run_stage1(list, fout);
#else
    if (strcmp(argv[1], "1") == 0) {
        run_stage1(list, fout);
    } else {
        run_stage2(list, fout);
    }
#endif

    fclose(fout);
    free_list(list);
    return 0;
}