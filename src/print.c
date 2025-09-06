#include <string.h>
#include "print.h"
#include "utils.h"

#define SAFE_STR(s) ((s) ? (s) : "")

/* Column headers in the exact dataset order */
static const char *HEADERS[35] = {
  "PFI","EZI_ADD","SRC_VERIF","PROPSTATUS","GCODEFEAT","LOC_DESC",
  "BLGUNTTYP","HSAUNITID","BUNIT_PRE1","BUNIT_ID1","BUNIT_SUF1",
  "BUNIT_PRE2","BUNIT_ID2","BUNIT_SUF2","FLOOR_TYPE","FLOOR_NO_1",
  "FLOOR_NO_2","BUILDING","COMPLEX","HSE_PREF1","HSE_NUM1","HSE_SUF1",
  "HSE_PREF2","HSE_NUM2","HSE_SUF2","DISP_NUM1","ROAD_NAME","ROAD_TYPE",
  "RD_SUF","LOCALITY","STATE","POSTCODE","ACCESSTYPE","x","y"
};

/* print a single record */
void print_record(FILE *out, const row_t *a){
    fprintf(out, "--> ");
    fprintf(out, "%s: %s || ",  HEADERS[0],  SAFE_STR(a->PFI));
    fprintf(out, "%s: %s || ",  HEADERS[1],  SAFE_STR(a->EZI_ADD));
    fprintf(out, "%s: %s || ",  HEADERS[2],  SAFE_STR(a->SRC_VERIF));
    fprintf(out, "%s: %s || ",  HEADERS[3],  SAFE_STR(a->PROPSTATUS));
    fprintf(out, "%s: %s || ",  HEADERS[4],  SAFE_STR(a->GCODEFEAT));
    fprintf(out, "%s: %s || ",  HEADERS[5],  SAFE_STR(a->LOC_DESC));
    fprintf(out, "%s: %s || ",  HEADERS[6],  SAFE_STR(a->BLGUNTTYP));
    fprintf(out, "%s: %s || ",  HEADERS[7],  SAFE_STR(a->HSAUNITID));
    fprintf(out, "%s: %s || ",  HEADERS[8],  SAFE_STR(a->BUNIT_PRE1));
    fprintf(out, "%s: %s || ",  HEADERS[9],  SAFE_STR(a->BUNIT_ID1));
    fprintf(out, "%s: %s || ",  HEADERS[10], SAFE_STR(a->BUNIT_SUF1));
    fprintf(out, "%s: %s || ",  HEADERS[11], SAFE_STR(a->BUNIT_PRE2));
    fprintf(out, "%s: %s || ",  HEADERS[12], SAFE_STR(a->BUNIT_ID2));
    fprintf(out, "%s: %s || ",  HEADERS[13], SAFE_STR(a->BUNIT_SUF2));
    fprintf(out, "%s: %s || ",  HEADERS[14], SAFE_STR(a->FLOOR_TYPE));
    fprintf(out, "%s: %s || ",  HEADERS[15], SAFE_STR(a->FLOOR_NO_1));
    fprintf(out, "%s: %s || ",  HEADERS[16], SAFE_STR(a->FLOOR_NO_2));
    fprintf(out, "%s: %s || ",  HEADERS[17], SAFE_STR(a->BUILDING));
    fprintf(out, "%s: %s || ",  HEADERS[18], SAFE_STR(a->COMPLEX));
    fprintf(out, "%s: %s || ",  HEADERS[19], SAFE_STR(a->HSE_PREF1));
    fprintf(out, "%s: %s || ",  HEADERS[20], SAFE_STR(a->HSE_NUM1));
    fprintf(out, "%s: %s || ",  HEADERS[21], SAFE_STR(a->HSE_SUF1));
    fprintf(out, "%s: %s || ",  HEADERS[22], SAFE_STR(a->HSE_PREF2));
    fprintf(out, "%s: %s || ",  HEADERS[23], SAFE_STR(a->HSE_NUM2));
    fprintf(out, "%s: %s || ",  HEADERS[24], SAFE_STR(a->HSE_SUF2));
    fprintf(out, "%s: %s || ",  HEADERS[25], SAFE_STR(a->DISP_NUM1));
    fprintf(out, "%s: %s || ",  HEADERS[26], SAFE_STR(a->ROAD_NAME));
    fprintf(out, "%s: %s || ",  HEADERS[27], SAFE_STR(a->ROAD_TYPE));
    fprintf(out, "%s: %s || ",  HEADERS[28], SAFE_STR(a->RD_SUF));
    fprintf(out, "%s: %s || ",  HEADERS[29], SAFE_STR(a->LOCALITY));
    fprintf(out, "%s: %s || ",  HEADERS[30], SAFE_STR(a->STATE));
    fprintf(out, "%s: %s || ",  HEADERS[31], SAFE_STR(a->POSTCODE));
    fprintf(out, "%s: %s || ",  HEADERS[32], SAFE_STR(a->ACCESSTYPE));
    fprintf(out, "%s: %Lf || ", HEADERS[33], a->x);
    fprintf(out, "%s: %Lf\n",   HEADERS[34], a->y);
}