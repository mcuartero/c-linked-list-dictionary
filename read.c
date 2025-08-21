#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"

#define DELIM ','

/* Duplicate a string safely */
static char *dup_string(const char *s) {
    if (!s) return NULL;
    char *copy = malloc(strlen(s) + 1);
    if (!copy) return NULL;
    strcpy(copy, s);
    return copy;
}

/* Split CSV line into tokens, preserving empty fields */
static int split_csv(char *line, char *tokens[], int max_fields) {
    int count = 0;
    char *start = line;
    char *p = line;

    while (*p && count < max_fields) {
        if (*p == DELIM) {
            *p = '\0';
            tokens[count++] = (*start) ? start : "";  // empty field -> ""
            start = p + 1;
        }
        p++;
    }

    if (count < max_fields) {
        tokens[count++] = (*start) ? start : "";
    }

    return count;
}

/* Read CSV into linked list */
node_t *read_csv(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    node_t *head = NULL, *tail = NULL;
    char line[1024];

    // Skip header
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return NULL;
    }

    // Process rows
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        row_t *row = parse_row(line);
        if (!row) continue;
        node_t *node = create_node(row);
        append_node(&head, &tail, node);
    }

    fclose(fp);
    return head;
}

/* Parse a single CSV row into a row_t */
row_t *parse_row(char *line) {
    if (!line) return NULL;
    row_t *row = calloc(1, sizeof(row_t));
    if (!row) return NULL;

    char *tokens[MAX_FIELDS] = {0};
    int i = split_csv(line, tokens, MAX_FIELDS);

    // Table of string fields in row_t
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

    int num_fields = sizeof(fields) / sizeof(fields[0]);

    // Assign string fields
    for (int j = 0; j < num_fields && j < i; j++) {
        *fields[j] = dup_string(tokens[j]);
    }

    // Handle x and y coordinates (last two fields)
    if (i > 33) row->x = strtold(tokens[33], NULL);
    if (i > 34) row->y = strtold(tokens[34], NULL);

    return row;
}

/* Create a new linked list node */
node_t *create_node(row_t *row) {
    if (!row) return NULL;
    node_t *node = malloc(sizeof(node_t));
    if (!node) return NULL;
    node->data = row;
    node->next = NULL;
    return node;
}

/* Append a node to the linked list */
void append_node(node_t **head, node_t **tail, node_t *node) {
    if (!node) return;
    if (!*head) *head = *tail = node;
    else {
        (*tail)->next = node;
        *tail = node;
    }
}

/* Free a row */
void free_row(row_t *row) {
    if (!row) return;

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

    int num_fields = sizeof(fields) / sizeof(fields[0]);
    for (int j = 0; j < num_fields; j++) {
        free(*fields[j]);
    }

    free(row);
}

/* Free entire linked list */
void free_list(node_t *head) {
    while (head) {
        node_t *next = head->next;
        free_row(head->data);
        free(head);
        head = next;
    }
}
