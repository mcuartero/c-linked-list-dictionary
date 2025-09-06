#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "read.h"
#include "list.h"
#include "search.h"
#include "print.h"
#include "patricia.h"

/* Error message for if the arguments provided are not valid */
static void usage(const char *p){
    fprintf(stderr, "Usage: %s stageNum <input.csv> <output.txt>\n", p);
    exit(1);
}

static void print_results(FILE *out, const search_stats_t *st) {
    if (st->result_count == 0) {
        fprintf(out, "NOTFOUND\n");
        return;
    }
    for (unsigned i = 0; i < st->result_count; ++i) {
        print_record(out, st->results[i]);
    }
}

/* Stage 1 functionality: Search by EZI_ADD */
static void run_stage1(node_t *list, FILE *fout) {
    char q[1024];
    
    while (fgets(q, sizeof(q), stdin)) {
        strip_newline(q); // Clean up the query string
        
        search_stats_t st;
        search_by_ezi_add(list, q, &st); // Perform search

        // Write results to output file
        fprintf(fout, "%s\n", q);
        if (st.result_count == 0) {
            fprintf(fout, "NOTFOUND\n");
        } else {
            for (unsigned int i = 0; i < st.result_count; i++) {
                print_record(fout, st.results[i]);
            }
        }

        // Print summary to stdout
        printf("%s --> %u records found - comparisons: b%llu n%u s%u\n",
               q, st.result_count,
               (unsigned long long)st.bit_comparisons,
               st.node_comparisons, st.string_comparisons);

        free(st.results); // Free results array
    }
}

static void run_stage2(node_t *list, FILE *fout) {
}

int main(int argc, char* argv[]){
    // Validate command line arguments
    if (argc != 4) usage(argv[0]);
    if (strcmp(argv[1], "1") != 0 &&
        strcmp(argv[1], "2") != 0) {
        usage(argv[0]);
    }

    const char *input_csv = argv[2];
    const char *output_txt = argv[3];

    // Read CSV file into linked list
    node_t *list = read_csv(input_csv);
    if (!list) {
        fprintf(stderr, "Error: Failed to read CSV file or file is empty\n");
        return 1;
    }

    // Open output file
    FILE *fout = fopen(output_txt, "w");
    if (!fout) { 
        perror("open output");  
        free_list(list); 
        return 1; 
    }

    int stage = atoi(argv[1]);
    
    // Execute appropriate stage
    switch (stage) {
        case 1:
            run_stage1(list, fout);
            break;
        case 2:
            run_stage2(list, fout);
            break;
        default:
            usage(argv[0]);
            break;
    }

    // Cleanup
    fclose(fout);
    free_list(list);
    return 0;
}