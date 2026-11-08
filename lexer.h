#pragma once
#include <stddef.h>
#include <stdio.h>

enum jasoon_token_type {
    TOK_L_BRACE,
    TOK_R_BRACE,
    TOK_L_BRACKET,
    TOK_R_BRACKET,
    TOK_COLON,
    TOK_COMMA,
    TOK_STRING,
    TOK_NUMBER,
    TOK_TRUE,
    TOK_FALSE,
    TOK_NULL,
};

typedef struct {
    char *start;
    char *end;
} jasoon_buffer_t;

typedef struct {
    enum jasoon_token_type type;
    char *value;
    size_t size;
} jasoon_token_t;

typedef struct {
    jasoon_buffer_t buffer;
    char *cursor;
    jasoon_token_t *tokens;
    size_t capacity;
    size_t count;
} jasoon_tokenizer_t;

jasoon_tokenizer_t jasoon_tokenize(char *buffer, size_t size);
