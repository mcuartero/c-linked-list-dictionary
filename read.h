#ifndef READ_H
#define READ_H

#include "row.h"

node_t *read_csv(const char *filename);
row_t *parse_row(char *line);
node_t *create_node(row_t *row);
void append_node(node_t **head, node_t **tail, node_t *node);
void free_row(row_t *row);
void free_list(node_t *head);

#endif
