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

/* show correct program usage */
static void usage(const char *prog){
    fprintf(stderr, "Usage: %s <stage> <input.csv> <output.txt>\n", prog);
    exit(1);
}

/*
 * The ENABLE_PATRICIA flag controls whether Stage 2 (Patricia tree search)
 * is compiled in. This lets the same main.c work for two builds:
 *   - dict1: no Patricia tree (stage 1 only)
 *   - dict2: includes Patricia tree (stage 1 + stage 2)
 */
#ifndef ENABLE_PATRICIA
#else

/* print results for Patricia tree search */
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

/* stage 1: search using linked list */
static void run_stage1(node_t *list, FILE *fout) {
    char q[1024];
    while (fgets(q, sizeof(q), stdin)) {
        strip_newline(q);

        search_stats_t st;
        search_by_ezi_add(list, q, &st);

        // write results to output file
        fprintf(fout, "%s\n", q);
        if (st.result_count == 0) {
            fprintf(fout, "NOTFOUND\n");
        } else {
            for (unsigned int i = 0; i < st.result_count; i++) {
                print_record(fout, st.results[i]);
            }
        }

        // print summary to stdout
        printf("%s --> %u records found - comparisons: b%llu n%u s%u\n",
               q, st.result_count,
               (unsigned long long)st.bit_comparisons,
               st.node_comparisons, st.string_comparisons);

        free(st.results);
    }
}

#ifdef ENABLE_PATRICIA

/* stage 2: search using Patricia tree */
static void run_stage2(node_t *list, FILE *fout) {
    patricia_tree_t *tree = create_patricia_tree();
    if (!tree) {
        fprintf(stderr, "Error: could not create Patricia tree\n");
        return;
    }

    // build Patricia tree from list of rows
    for (node_t *cur = list; cur; cur = cur->next) {
        if (cur->data && cur->data->EZI_ADD) {
            insert_into_patricia(tree, cur->data->EZI_ADD, cur->data);
        }
    }

    char q[1024];
    while (fgets(q, sizeof(q), stdin)) {
        strip_newline(q);

        search_stats_t st;
        search_patricia(tree, q, &st, 1);  // spellcheck enabled

        // write results to output file
        fprintf(fout, "%s\n", q);
        print_results(fout, &st);

        // print summary to stdout
        printf("%s --> %u records found - comparisons: b%llu n%u s%u\n",
               q, st.result_count,
               (unsigned long long)st.bit_comparisons,
               st.node_comparisons, st.string_comparisons);

        free(st.results);
    }

    free_patricia_tree(tree);
}
#endif

/* main: chooses stage, reads CSV, runs search, writes output */
int main(int argc, char *argv[]) {
    if (argc != 4) usage(argv[0]);

#ifndef ENABLE_PATRICIA
    // If Patricia is not enabled, only stage 1 is valid
    if (strcmp(argv[1], "1") != 0) {
        fprintf(stderr, "This build excludes Patricia (stage 2). Use dict2.\n");
        usage(argv[0]);
    }
#else
    // If Patricia is enabled, allow stage 1 or stage 2
    if (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0) {
        usage(argv[0]);
    }
#endif

    const char *input_csv  = argv[2];
    const char *output_txt = argv[3];

    // read CSV into linked list
    node_t *list = read_csv(input_csv);
    if (!list) {
        fprintf(stderr, "Error: failed to read CSV or file is empty\n");
        return 1;
    }

    // open output file
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
