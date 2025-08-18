#ifndef ROW_H
#define ROW_H

#define MAX_FIELDS 35
#define MAX_FIELD_LEN 127

typedef struct row_t {
    char *PFI;
    char *EZI_ADD;
    char *SRC_VERIF;
    char *PROPSTATUS;
    char *GCODEFEAT;
    char *LOC_DESC;
    char *BLGUNTTYP;
    char *HSAUNITID;
    char *BUNIT_PRE1;
    char *BUNIT_ID1;
    char *BUNIT_SUF1;
    char *BUNIT_PRE2;
    char *BUNIT_ID2;
    char *BUNIT_SUF2;
    char *FLOOR_TYPE;
    char *FLOOR_NO_1;
    char *FLOOR_NO_2;
    char *BUILDING;
    char *COMPLEX;
    char *HSE_PREF1;
    char *HSE_NUM1;
    char *HSE_SUF1;
    char *HSE_PREF2;
    char *HSE_NUM2;
    char *HSE_SUF2;
    char *DISP_NUM1;
    char *ROAD_NAME;
    char *ROAD_TYPE;
    char *RD_SUF;
    char *LOCALITY;
    char *STATE;
    char *POSTCODE;
    char *ACCESSTYPE;

    long double x;
    long double y;
} row_t;

typedef struct node_t {
    row_t *data;
    struct node_t *next;
}

row_t *create_row(char **fields);
void free_row(row_t *row);

node_t *create_node(row_t *row);
void free_list(node_t *head);

#endif