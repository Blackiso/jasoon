#pragma once
#include "lexer.h"
#include <stddef.h>

typedef struct jasoon_node_t jasoon_node_t;
typedef struct jasoon_array_t jasoon_array_t;
typedef struct jasoon_object_t jasoon_object_t;
typedef struct jasoon_object_child_t jasoon_object_child_t;
typedef struct jasoon_string_t jasoon_string_t;
typedef struct jasoon_parser_t jasoon_parser_t;
typedef struct jasoon_t jasoon_t;
typedef enum jasoon_node_type jasoon_node_type_t;

enum jasoon_node_type {
    JASOON_OBJECT,
    JASOON_ARRAY,
    JASOON_STRING,
    JASOON_NUMBER,
    JASOON_FALSE,
    JASOON_TRUE,
    JASOON_NULL,
};

struct jasoon_array_t {
    jasoon_node_t **children;
    size_t count;
    size_t capacity;
};

struct jasoon_object_child_t {
    char *key;
    size_t key_size;
    jasoon_node_t *value;
};

struct jasoon_object_t {
    jasoon_object_child_t *children;
    size_t count;
    size_t capacity;
};

struct jasoon_string_t {
    char *string;
    size_t size;
};

struct jasoon_node_t {
    jasoon_node_type_t type;

    union {
        jasoon_object_t object;
        jasoon_array_t array;
        double number;
        jasoon_string_t string;
    };
};

struct jasoon_parser_t {
    jasoon_tokenizer_t tokens;
    jasoon_token_t *cursor;
};

struct jasoon_t {
    jasoon_parser_t parser;
    jasoon_node_t *node;
};

jasoon_node_t *jasoon_parse_object_node(jasoon_parser_t *parser);
jasoon_node_t *jasoon_parse_array_node(jasoon_parser_t *parser);
jasoon_node_t *jasoon_parse_string_node(jasoon_token_t *token);
jasoon_node_t *jasoon_parse_number_node(jasoon_token_t *token);
jasoon_node_t *jasoon_parse_literal_node(jasoon_node_type_t type);
jasoon_t jasoon_parse(char *json_string, size_t size);

void jasoon_free_object(jasoon_object_t object, jasoon_buffer_t buffer);
void jasoon_free_array(jasoon_array_t array, jasoon_buffer_t buffer);
void jasoon_free_node(jasoon_node_t *node, jasoon_buffer_t buffer);
void jasoon_free(jasoon_t json);
