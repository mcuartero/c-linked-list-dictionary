#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rows.h"

void print_row(const row_t *row);
void print_list(const struct node_t *head);

int main() {
    FILE *fp = fopen("tests/dataset_2.csv", "r");
    if (!fp) { perror("fopen"); return 1; }

    char line[512];
    node_t *head = NULL, *tail = NULL;

    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 1; }

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0'; 
        row_t *row = parse_row(line);
        node_t *node = create_node(row);
        append_node(&head, &tail, node);
    }

    fclose(fp);
    

    node_t *cur = head;
    while (cur) {
        print_row(cur->data);
        cur = cur->next;
    }

    free_list(head);
    return 0;
}

void print_row(const row_t *row) {
    if (!row) return;

    printf("--> PFI: %s || EZI_ADD: %s || SRC_VERIF: %s || PROPSTATUS: %s || "
           "GCODEFEAT: %s || LOC_DESC: %s || BLGUNTTYP: %s || HSAUNITID: %s || "
           "BUNIT_PRE1: %s || BUNIT_ID1: %s || BUNIT_SUF1: %s || BUNIT_PRE2: %s || "
           "BUNIT_ID2: %s || BUNIT_SUF2: %s || FLOOR_TYPE: %s || FLOOR_NO_1: %s || "
           "FLOOR_NO_2: %s || BUILDING: %s || COMPLEX: %s || HSE_PREF1: %s || "
           "HSE_NUM1: %s || HSE_SUF1: %s || HSE_PREF2: %s || HSE_NUM2: %s || "
           "HSE_SUF2: %s || DISP_NUM1: %s || ROAD_NAME: %s || ROAD_TYPE: %s || "
           "RD_SUF: %s || LOCALITY: %s || STATE: %s || POSTCODE: %s || ACCESSTYPE: %s || "
           "x: %.5Lf || y: %.5Lf ||\n",
           row->PFI, row->EZI_ADD, row->SRC_VERIF, row->PROPSTATUS,
           row->GCODEFEAT, row->LOC_DESC, row->BLGUNTTYP, row->HSAUNITID,
           row->BUNIT_PRE1, row->BUNIT_ID1, row->BUNIT_SUF1, row->BUNIT_PRE2,
           row->BUNIT_ID2, row->BUNIT_SUF2, row->FLOOR_TYPE, row->FLOOR_NO_1,
           row->FLOOR_NO_2, row->BUILDING, row->COMPLEX, row->HSE_PREF1,
           row->HSE_NUM1, row->HSE_SUF1, row->HSE_PREF2, row->HSE_NUM2,
           row->HSE_SUF2, row->DISP_NUM1, row->ROAD_NAME, row->ROAD_TYPE,
           row->RD_SUF, row->LOCALITY, row->STATE, row->POSTCODE,
           row->ACCESSTYPE, row->x, row->y);
}

void print_list(const node_t *head) {
    const node_t *cur = head;
    while (cur) {
        print_row(cur->data);
        cur = cur->next;
    }
}