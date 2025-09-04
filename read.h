#ifndef READ_H
#define READ_H

#include "row.h"

// Function to read CSV file and convert to linked list
node_t *read_csv(const char *filename);

// Function to create a new linked list node
node_t *create_node(row_t *row);

// Function to append a node to the end of linked list
void append_node(node_t **head, node_t **tail, node_t *node);

// Function to free memory allocated for a single row
void free_row(row_t *row);

// Function to free entire linked list and all rows
void free_list(node_t *head);

#endif