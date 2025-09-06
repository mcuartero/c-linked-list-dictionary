#ifndef LIST_H
#define LIST_H
#include "row.h"

node_t *create_node(row_t *row);
void append_node(node_t **head, node_t **tail, node_t *node);
void free_list(node_t *head);

#endif // LIST_H