#pragma once
#include "lexer.h"
#include "parser.h"
#include <stddef.h>

void *relocate(void *array, size_t size, size_t size_of_type);
void append(void **array, size_t *count, size_t *capacity, void *item, size_t size_of_type);
void size_to_fit(void **array, size_t count, size_t size_of_type);
void invalid(int code);
void print_token(jasoon_token_t *token);
void jasoon_print_node(jasoon_node_t *node, int depth);
