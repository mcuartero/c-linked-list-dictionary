#ifndef ROW_H
#define ROW_H

// Maximum number of fields in a CSV row
#define MAX_FIELDS 35
// Maximum length for each field
#define MAX_FIELD_LEN 127

// Structure representing a single row/record from the CSV file
typedef struct row_t {
    // String fields from the CSV (35 fields total)
    char *PFI;              // Property FIeld identifier
    char *EZI_ADD;          // Easy Address - main search field
    char *SRC_VERIF;        // Source Verification
    char *PROPSTATUS;       // Property Status
    char *GCODEFEAT;        // Geocode Feature
    char *LOC_DESC;         // Location Description
    char *BLGUNTTYP;        // Building Unit Type
    char *HSAUNITID;        // HSA Unit ID
    char *BUNIT_PRE1;       // Building Unit Prefix 1
    char *BUNIT_ID1;        // Building Unit ID 1
    char *BUNIT_SUF1;       // Building Unit Suffix 1
    char *BUNIT_PRE2;       // Building Unit Prefix 2
    char *BUNIT_ID2;        // Building Unit ID 2
    char *BUNIT_SUF2;       // Building Unit Suffix 2
    char *FLOOR_TYPE;       // Floor Type
    char *FLOOR_NO_1;       // Floor Number 1
    char *FLOOR_NO_2;       // Floor Number 2
    char *BUILDING;         // Building name/number
    char *COMPLEX;          // Complex name
    char *HSE_PREF1;        // House Prefix 1
    char *HSE_NUM1;         // House Number 1
    char *HSE_SUF1;         // House Suffix 1
    char *HSE_PREF2;        // House Prefix 2
    char *HSE_NUM2;         // House Number 2
    char *HSE_SUF2;         // House Suffix 2
    char *DISP_NUM1;        // Display Number 1
    char *ROAD_NAME;        // Road Name
    char *ROAD_TYPE;        // Road Type
    char *RD_SUF;           // Road Suffix
    char *LOCALITY;         // Locality/Suburb
    char *STATE;            // State
    char *POSTCODE;         // Postcode
    char *ACCESSTYPE;       // Access Type
    
    // Coordinate fields (longitude and latitude)
    long double x;          // Longitude coordinate
    long double y;          // Latitude coordinate
} row_t;

// Linked list node structure for storing rows
typedef struct node_t {
    row_t *data;            // Pointer to row data
    struct node_t *next;    // Pointer to next node in list
} node_t;

void free_row(row_t *row);

#endif