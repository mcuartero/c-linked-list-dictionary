#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"
#include "csv.h"

/* Read CSV file and convert to linked list */
node_t *read_csv(const char *filename) {
    FILE *fp = fopen(filename, "r");        // Open file for reading
    if (!fp) return NULL;                   // Return NULL if file open fails

    node_t *head = NULL, *tail = NULL;      // Linked list head and tail pointers
    char line[1024];                        // Buffer for reading lines

    // Skip header row (first line)
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return NULL;                        // Return NULL if file is empty
    }

    // Process each data row
    while (fgets(line, sizeof(line), fp)) {
        // Remove newline/carriage return characters
        line[strcspn(line, "\r\n")] = '\0';
        row_t *row = parse_row(line);       // Parse line into row structure
        if (!row) continue;                 // Skip if parsing failed
        node_t *node = create_node(row);    // Create linked list node
        append_node(&head, &tail, node);    // Add node to end of list
    }

    fclose(fp);                             // Close the file
    return head;                            // Return head of linked list
}


/* Create a new linked list node containing row data */
node_t *create_node(row_t *row) {
    if (!row) return NULL;                  // Return NULL for invalid input
    
    node_t *node = malloc(sizeof(node_t));  // Allocate node memory
    if (!node) return NULL;                 // Return NULL if allocation fails
    
    node->data = row;                       // Assign row data
    node->next = NULL;                      // Initialize next pointer to NULL
    return node;
}

/* Append a node to the end of the linked list */
void append_node(node_t **head, node_t **tail, node_t *node) {
    if (!node) return;                      // Do nothing for NULL node
    
    if (!*head) {                           // If list is empty
        *head = *tail = node;               // Set both head and tail to new node
    } else {
        (*tail)->next = node;               // Add node to end of list
        *tail = node;                       // Update tail pointer
    }
}

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

/* Free entire linked list and all associated row data */
void free_list(node_t *head) {
    while (head) {
        node_t *next = head->next;          // Save pointer to next node
        free_row(head->data);               // Free row data
        free(head);                         // Free node itself
        head = next;                        // Move to next node
    }
}