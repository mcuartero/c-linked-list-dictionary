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
