#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "csv.h"

#define DELIM ","

/* Safely duplicate a string with memory allocation */
static char *dup_string(const char *s) {
    if (!s) return NULL;                    // Handle NULL input
    char *copy = malloc(strlen(s) + 1);     // Allocate memory for copy
    if (!copy) return NULL;                 // Check allocation success
    strcpy(copy, s);                        // Copy the string
    return copy;
}

/* Split CSV line into tokens, preserving empty fields */
static int split_csv(char *line, char *tokens[], int max_fields) {
    int count = 0;
    char *start = line;                     // Start of current field
    char *p = line;                         // Current position in line

    // Process each character in the line
    while (*p && count < max_fields) {
        if (*p == DELIM) {                  // Found field delimiter
            *p = '\0';                      // Terminate current field
            tokens[count++] = (*start) ? start : "";  // Store field (empty if needed)
            start = p + 1;                  // Move to next field start
        }
        p++;
    }

    // Handle the last field
    if (count < max_fields) {
        tokens[count++] = (*start) ? start : "";
    }

    return count;
}

/* Parse a single CSV line into row_t structure */
row_t *parse_row(char *line) {
    if (!line) return NULL;                 // Return NULL for invalid input
    
    // Allocate and initialize row structure
    row_t *row = calloc(1, sizeof(row_t));
    if (!row) return NULL;                  // Return NULL if allocation fails

    char *tokens[MAX_FIELDS] = {0};         // Array for field tokens
    int i = split_csv(line, tokens, MAX_FIELDS);  // Split line into tokens

    // Array of pointers to string fields in row_t for easy assignment
    char **fields[] = {
        &row->PFI, &row->EZI_ADD, &row->SRC_VERIF, &row->PROPSTATUS,
        &row->GCODEFEAT, &row->LOC_DESC, &row->BLGUNTTYP, &row->HSAUNITID,
        &row->BUNIT_PRE1, &row->BUNIT_ID1, &row->BUNIT_SUF1, &row->BUNIT_PRE2,
        &row->BUNIT_ID2, &row->BUNIT_SUF2, &row->FLOOR_TYPE, &row->FLOOR_NO_1,
        &row->FLOOR_NO_2, &row->BUILDING, &row->COMPLEX, &row->HSE_PREF1,
        &row->HSE_NUM1, &row->HSE_SUF1, &row->HSE_PREF2, &row->HSE_NUM2,
        &row->HSE_SUF2, &row->DISP_NUM1, &row->ROAD_NAME, &row->ROAD_TYPE,
        &row->RD_SUF, &row->LOCALITY, &row->STATE, &row->POSTCODE,
        &row->ACCESSTYPE
    };

    int num_fields = sizeof(fields) / sizeof(fields[0]);  // Calculate number of string fields

    // Assign string fields from tokens
    for (int j = 0; j < num_fields && j < i; j++) {
        *fields[j] = dup_string(tokens[j]);  // Duplicate and assign each token
    }

    // Handle numeric coordinate fields (last two fields)
    if (i > 33) row->x = strtold(tokens[33], NULL);  // Convert x coordinate
    if (i > 34) row->y = strtold(tokens[34], NULL);  // Convert y coordinate

    return row;
}

