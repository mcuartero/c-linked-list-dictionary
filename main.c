#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "read.h"
#include "search.h"

/* Column headers in the exact dataset order */
static const char *HEADERS[35] = {
  "PFI","EZI_ADD","SRC_VERIF","PROPSTATUS","GCODEFEAT","LOC_DESC",
  "BLGUNTTYP","HSAUNITID","BUNIT_PRE1","BUNIT_ID1","BUNIT_SUF1",
  "BUNIT_PRE2","BUNIT_ID2","BUNIT_SUF2","FLOOR_TYPE","FLOOR_NO_1",
  "FLOOR_NO_2","BUILDING","COMPLEX","HSE_PREF1","HSE_NUM1","HSE_SUF1",
  "HSE_PREF2","HSE_NUM2","HSE_SUF2","DISP_NUM1","ROAD_NAME","ROAD_TYPE",
  "RD_SUF","LOCALITY","STATE","POSTCODE","ACCESSTYPE","x","y"
};

/* Error message for if the arguments provided are not valid */
static void usage(const char *p){
    fprintf(stderr, "Usage: %s stageNum <input.csv> <output.txt>\n", p);
    exit(1);
}

/* print one record as: --> FIELD: value || FIELD: value ... */
static void print_record(FILE *out, const row_t *a){
    fprintf(out, "--> ");
    fprintf(out, "%s: %s || ",  HEADERS[0],  a->PFI);
    fprintf(out, "%s: %s || ",  HEADERS[1],  a->EZI_ADD);
    fprintf(out, "%s: %s || ",  HEADERS[2],  a->SRC_VERIF);
    fprintf(out, "%s: %s || ",  HEADERS[3],  a->PROPSTATUS);
    fprintf(out, "%s: %s || ",  HEADERS[4],  a->GCODEFEAT);
    fprintf(out, "%s: %s || ",  HEADERS[5],  a->LOC_DESC);
    fprintf(out, "%s: %s || ",  HEADERS[6],  a->BLGUNTTYP);
    fprintf(out, "%s: %s || ",  HEADERS[7],  a->HSAUNITID);
    fprintf(out, "%s: %s || ",  HEADERS[8],  a->BUNIT_PRE1);
    fprintf(out, "%s: %s || ",  HEADERS[9],  a->BUNIT_ID1);
    fprintf(out, "%s: %s || ",  HEADERS[10], a->BUNIT_SUF1);
    fprintf(out, "%s: %s || ",  HEADERS[11], a->BUNIT_PRE2);
    fprintf(out, "%s: %s || ",  HEADERS[12], a->BUNIT_ID2);
    fprintf(out, "%s: %s || ",  HEADERS[13], a->BUNIT_SUF2);
    fprintf(out, "%s: %s || ",  HEADERS[14], a->FLOOR_TYPE);
    fprintf(out, "%s: %s || ",  HEADERS[15], a->FLOOR_NO_1);
    fprintf(out, "%s: %s || ",  HEADERS[16], a->FLOOR_NO_2);
    fprintf(out, "%s: %s || ",  HEADERS[17], a->BUILDING);
    fprintf(out, "%s: %s || ",  HEADERS[18], a->COMPLEX);
    fprintf(out, "%s: %s || ",  HEADERS[19], a->HSE_PREF1);
    fprintf(out, "%s: %s || ",  HEADERS[20], a->HSE_NUM1);
    fprintf(out, "%s: %s || ",  HEADERS[21], a->HSE_SUF1);
    fprintf(out, "%s: %s || ",  HEADERS[22], a->HSE_PREF2);
    fprintf(out, "%s: %s || ",  HEADERS[23], a->HSE_NUM2);
    fprintf(out, "%s: %s || ",  HEADERS[24], a->HSE_SUF2);
    fprintf(out, "%s: %s || ",  HEADERS[25], a->DISP_NUM1);
    fprintf(out, "%s: %s || ",  HEADERS[26], a->ROAD_NAME);
    fprintf(out, "%s: %s || ",  HEADERS[27], a->ROAD_TYPE);
    fprintf(out, "%s: %s || ",  HEADERS[28], a->RD_SUF);
    fprintf(out, "%s: %s || ",  HEADERS[29], a->LOCALITY);
    fprintf(out, "%s: %s || ",  HEADERS[30], a->STATE);
    fprintf(out, "%s: %s || ",  HEADERS[31], a->POSTCODE);
    fprintf(out, "%s: %s || ",  HEADERS[32], a->ACCESSTYPE);
    fprintf(out, "%s: %Lf || ",  HEADERS[33], a->x);
    fprintf(out, "%s: %Lf\n",   HEADERS[34], a->y);
}

/* Strip newline and carriage return characters from string */
static void strip_newline(char *str) {
    size_t n = strlen(str);
    while (n > 0) {
        char last = str[n - 1];
        if (last == '\n' || last == '\r') {
            n--;
            str[n] = '\0';
        } else {
            break;
        }
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

int main(int argc, char* argv[]){
    // Validate command line arguments
    if (argc != 4) usage(argv[0]);
    if (strcmp(argv[1], "1") != 0 &&
        strcmp(argv[1], "2") != 0 &&
        strcmp(argv[1], "3") != 0) {
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
        // case 2:  
        //     run_stage2(list, fout)
        // ...
    }

    // Cleanup
    fclose(fout);
    free_list(list);
    return 0;
}
