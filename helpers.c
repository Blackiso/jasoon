#include "lexer.h"
#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *relocate(void *array, size_t size, size_t size_of_type) {
    void *new_obj = realloc(array, size * size_of_type);
    if (new_obj == NULL) {
        perror("Failed to reallocate memory");
        exit(1);
    }
    return new_obj;
}

void append(void **array, size_t *count, size_t *capacity, void *item, size_t size_of_type) {
    if (*capacity == 0) {
        *capacity = 10;
        *array = malloc(size_of_type * (*capacity));
    } else if (*count >= *capacity) {
        *capacity *= 2;
        *array = relocate(*array, *capacity, size_of_type);
    }

    memcpy((char *)*array + (*count * size_of_type), item, size_of_type);

    (*count)++;
}

void size_to_fit(void **array, size_t count, size_t size_of_type) {
    *array = relocate(*array, count, size_of_type);
}

void invalid(int code) {
    fprintf(stderr, "\nERROR CODE |%d| INVALID JSON!!\n", code);
    exit(1);
}

void print_token(jasoon_token_t *token) {
    printf("TOKEN: %.*s\n", (int)token->size, token->value);
}

void jasoon_print_node(jasoon_node_t *node, int depth) {
    if (!node) {
        return;
    }

    switch (node->type) {
    case JASOON_OBJECT: {
        printf("{\n");

        for (int i = 0; i < node->object.count; i++) {
            jasoon_object_child_t child = node->object.children[i];

            for (int d = 0; d < depth + 1; d++) {
                printf("  ");
            }

            printf("\"%.*s\": ", (int)child.key_size, child.key);

            jasoon_print_node(child.value, depth + 1);

            if (i < node->object.count - 1) {
                printf(",");
            }

            printf("\n");
        }

        for (int d = 0; d < depth; d++) {
            printf("  ");
        }

        printf("}");
        break;
    }
    case JASOON_ARRAY: {
        printf("[\n");
        for (int i = 0; i < node->array.count; i++) {
            jasoon_node_t *item = node->array.children[i];

            for (int d = 0; d < depth + 1; d++) {
                printf("  ");
            }

            jasoon_print_node(item, depth + 1);

            if (i < node->array.count - 1) {
                printf(",");
            }

            printf("\n");
        }

        for (int d = 0; d < depth; d++) {
            printf("  ");
        }

        printf("]");
        break;
    }
    case JASOON_STRING: {
        printf("\"%.*s\"", (int)node->string.size, node->string.string);
        break;
    }
    case JASOON_NUMBER: {
        printf("%g", node->number);
        break;
    }
    case JASOON_TRUE: {
        printf("true");
        break;
    }
    case JASOON_FALSE: {
        printf("false");
        break;
    }
    case JASOON_NULL: {
        printf("null");
        break;
    }
    default: {
        printf("unknown");
        break;
    }
    }
}
