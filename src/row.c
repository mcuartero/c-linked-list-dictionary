#include <stdlib.h>
#include "row.h"

/* Free memory allocated for a single row */
void free_row(row_t *row) {
    if (!row) return;                       // Do nothing for NULL row

    // Array of pointers to all string fields for easy deallocation
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
    
    // Free all string fields
    for (int j = 0; j < num_fields; j++) {
        free(*fields[j]);                   // Free each string field
    }

    free(row);                              // Free the row structure itself
}