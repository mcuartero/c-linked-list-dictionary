#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"

#define DELIM ","

static char *dup_string(const char *s) {
    size_t len = s ? strnlen(s, MAX_FIELD_LEN) : 0;
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    if (s && len > 0) memcpy(copy, s, len);
    copy[len] = '\0';
    return copy;
}

node_t *read_csv(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    node_t *head = NULL, *tail = NULL;
    char line[512];

    // Skip header
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return NULL;
    }

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

row_t *parse_row(char *line) {
    if (!line) return NULL;
    row_t *row = calloc(1, sizeof(row_t));
    if (!row) return NULL;

    char *tokens[MAX_FIELDS] = {0};
    int i = 0;
    char *p = strtok(line, DELIM);
    while (p && i < MAX_FIELDS) {
        tokens[i++] = p;
        p = strtok(NULL, DELIM);
    }

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

    int num_tokens = i;
    for (int j = 0; j < MAX_FIELDS - 2; j++) {
        if (j < num_tokens - 2) *fields[j] = dup_string(tokens[j]);
        else *fields[j] = dup_string("");
    }

    if (num_tokens >= 2) {
        row->x = strtold(tokens[num_tokens - 2], NULL);
        row->y = strtold(tokens[num_tokens - 1], NULL);
    } else {
        row->x = 0.0;
        row->y = 0.0;
    }

    return row;
}

node_t *create_node(row_t *row) {
    if (!row) return NULL;
    node_t *node = malloc(sizeof(node_t));
    if (!node) return NULL;
    node->data = row;
    node->next = NULL;
    return node;
}

void append_node(node_t **head, node_t **tail, node_t *node) {
    if (!node) return;
    if (!*head) *head = *tail = node;
    else {
        (*tail)->next = node;
        *tail = node;
    }
}

void free_row(row_t *row) {
    if (!row) return;
    char *fields[] = {
        row->PFI, row->EZI_ADD, row->SRC_VERIF, row->PROPSTATUS,
        row->GCODEFEAT, row->LOC_DESC, row->BLGUNTTYP, row->HSAUNITID,
        row->BUNIT_PRE1, row->BUNIT_ID1, row->BUNIT_SUF1, row->BUNIT_PRE2,
        row->BUNIT_ID2, row->BUNIT_SUF2, row->FLOOR_TYPE, row->FLOOR_NO_1,
        row->FLOOR_NO_2, row->BUILDING, row->COMPLEX, row->HSE_PREF1,
        row->HSE_NUM1, row->HSE_SUF1, row->HSE_PREF2, row->HSE_NUM2,
        row->HSE_SUF2, row->DISP_NUM1, row->ROAD_NAME, row->ROAD_TYPE,
        row->RD_SUF, row->LOCALITY, row->STATE, row->POSTCODE,
        row->ACCESSTYPE
    };
    for (int i = 0; i < MAX_FIELDS - 2; i++) free(fields[i]);
    free(row);
}

void free_list(node_t *head) {
    while (head) {
        node_t *next = head->next;
        free_row(head->data);
        free(head);
        head = next;
    }
}
