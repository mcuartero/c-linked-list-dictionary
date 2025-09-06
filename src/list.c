#include <stdlib.h>
#include "list.h"

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

/* Free entire linked list and all associated row data */
void free_list(node_t *head) {
    while (head) {
        node_t *next = head->next;          // Save pointer to next node
        free_row(head->data);               // Free row data
        free(head);                         // Free node itself
        head = next;                        // Move to next node
    }
}