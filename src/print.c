#include <string.h>
#include "print.h"

/* Column headers in the exact dataset order */
static const char *HEADERS[35] = {
  "PFI","EZI_ADD","SRC_VERIF","PROPSTATUS","GCODEFEAT","LOC_DESC",
  "BLGUNTTYP","HSAUNITID","BUNIT_PRE1","BUNIT_ID1","BUNIT_SUF1",
  "BUNIT_PRE2","BUNIT_ID2","BUNIT_SUF2","FLOOR_TYPE","FLOOR_NO_1",
  "FLOOR_NO_2","BUILDING","COMPLEX","HSE_PREF1","HSE_NUM1","HSE_SUF1",
  "HSE_PREF2","HSE_NUM2","HSE_SUF2","DISP_NUM1","ROAD_NAME","ROAD_TYPE",
  "RD_SUF","LOCALITY","STATE","POSTCODE","ACCESSTYPE","x","y"
};

/* Strip newline and carriage return characters from string */
void strip_newline(char *str) {
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

/* print a single record */
void print_record(FILE *out, const row_t *a){
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
