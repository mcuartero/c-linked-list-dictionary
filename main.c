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

int main(int argc, char* argv[]){
    if(argc != 4) usage(argv[0]);
    if(strcmp(argv[1], "1") != 0 &&
       strcmp(argv[1], "2") != 0 &&
       strcmp(argv[1], "3") != 0) {
        usage(argv[0]);
    }

    const char *input_csv = argv[2];
    const char *output_txt = argv[3];

    node_t *list = read_csv(input_csv); // Reading CSV into linked list

    if (!list) {
        fprintf(stderr, "Error: Failed to read CSV file or file is empty\n");
        return 1;
    }

    FILE *fout = fopen(output_txt, "w");
    if(!fout){ 
        perror("open output");  
        free_list(list); 
        return 1; 
    }

    /* Read queries from stdin until EOF, queries can be an empty string */
    char q[1024];
    /* If stage is 1 */
    if(atoi(argv[1]) == 1){
        while(fgets(q, sizeof(q), stdin)){
            /* Strip newline(s) */
            size_t n = strlen(q);
            /* Keep removing trailing newline or carriage return characters */
            while (n > 0) {
                char last = q[n - 1];   // Look at the last character

                if (last == '\n' || last == '\r') {
                    n--;                // Shrink the string length
                    q[n] = '\0';        // Put string terminator at the new end
                } else {
                    break;              // Stop if it's not \n or \r
                }
            }

            search_stats_t st;
            search_by_ezi_add(list, q, &st);

            /* Output file */
            fprintf(fout, "%s\n", q);
            if(st.result_count == 0){
                fprintf(fout, "NOTFOUND\n");
            } else{
                for(unsigned int i=0 ; i < st.result_count ; i++){
                    print_record(fout, st.results[i]);
                }
            }

            /* Stdout summary line */
            printf("%s --> %u records found - comparisons: b%llu n%u s%u\n",
                q, st.result_count,
                (unsigned long long)st.bit_comparisons,
                st.node_comparisons, st.string_comparisons);

            free(st.results); // Freeing results
        }
    }
    /* Freeing list pointer and closing files */
    fclose(fout);
    free_list(list);
    return 0;
}