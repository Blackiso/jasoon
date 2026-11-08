#include "parser.h"
#include "helpers.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

jasoon_t jasoon_parse(char *json_string, size_t size) {
    jasoon_t json;
    jasoon_parser_t parser;
    jasoon_node_t *node;

    parser.tokens = jasoon_tokenize(json_string, size);
    parser.cursor = parser.tokens.tokens;

    if (parser.cursor->type == TOK_L_BRACE) {
        node = jasoon_parse_object_node(&parser);
    } else if (parser.cursor->type == TOK_L_BRACKET) {
        node = jasoon_parse_array_node(&parser);
    } else {
        invalid(30);
    }

    json.node = node;
    json.parser = parser;

    return json;
}

jasoon_node_t *jasoon_parse_object_node(jasoon_parser_t *parser) {

    jasoon_token_t *end = (parser->tokens.tokens + (parser->tokens.count - 1));
    jasoon_node_t *node = malloc(sizeof(jasoon_node_t));
    jasoon_object_t object = {
        .capacity = 0,
        .count = 0,
    };

    while (parser->cursor <= end && parser->cursor->type != TOK_R_BRACE) {
        parser->cursor++;

        if (parser->cursor->type == TOK_R_BRACE) {
            continue;
        }

        jasoon_object_child_t child;
        int isKey = 1;
        int isRunning = 1;

        while (parser->cursor <= end && parser->cursor->type != TOK_COMMA && isRunning) {

            switch (parser->cursor->type) {

            case TOK_R_BRACE:
                isRunning = 0;
                continue;
                break;
            case TOK_L_BRACE:
                child.value = jasoon_parse_object_node(parser);
                continue;

            case TOK_COLON:
                isKey = 0;
                break;

            case TOK_STRING:
                if (isKey) {
                    child.key = parser->cursor->value;
                    child.key_size = parser->cursor->size;
                } else {
                    child.value = jasoon_parse_string_node(parser->cursor);
                }
                break;
            case TOK_NUMBER:
                child.value = jasoon_parse_number_node(parser->cursor);
                break;

            case TOK_L_BRACKET:
                child.value = jasoon_parse_array_node(parser);
                continue;

            case TOK_R_BRACKET:
                break;

            case TOK_NULL:
                child.value = jasoon_parse_literal_node(JASOON_NULL);
                break;
            case TOK_FALSE:
                child.value = jasoon_parse_literal_node(JASOON_FALSE);
                break;
            case TOK_TRUE:
                child.value = jasoon_parse_literal_node(JASOON_TRUE);
                break;
            default:
                printf("\nTYPE: %c", parser->cursor->type);
                invalid(60);
                break;
            }

            parser->cursor++;
        }

        append((void **)&object.children, &(object.count), &(object.capacity), (void *)&child, sizeof(jasoon_object_child_t));
    }

    size_to_fit((void **)&object.children, object.count, sizeof(jasoon_object_child_t));

    node->type = JASOON_OBJECT;
    node->object = object;

    parser->cursor++;
    return node;
}

jasoon_node_t *jasoon_parse_array_node(jasoon_parser_t *parser) {
    jasoon_token_t *end = (parser->tokens.tokens + (parser->tokens.count - 1));
    jasoon_node_t *node = malloc(sizeof(jasoon_node_t));
    jasoon_array_t array = {
        .count = 0,
        .capacity = 0,
    };

    while (parser->cursor <= end && parser->cursor->type != TOK_R_BRACKET) {
        parser->cursor++;

        jasoon_node_t *item;

        switch (parser->cursor->type) {

        case TOK_R_BRACKET:
            continue;
        case TOK_L_BRACKET:
            item = jasoon_parse_array_node(parser);
            break;
        case TOK_L_BRACE:
            item = jasoon_parse_object_node(parser);
            break;
        case TOK_STRING:
            item = jasoon_parse_string_node(parser->cursor);
            break;
        case TOK_NUMBER:
            item = jasoon_parse_number_node(parser->cursor);
            break;
        case TOK_NULL:
            item = jasoon_parse_literal_node(JASOON_NULL);
            break;
        case TOK_FALSE:
            item = jasoon_parse_literal_node(JASOON_FALSE);
            break;
        case TOK_TRUE:
            item = jasoon_parse_literal_node(JASOON_TRUE);
            break;
        case TOK_COMMA:
            continue;
        default:
            printf("\nTYPE: %c", parser->cursor->type);
            invalid(60);
            break;
        }

        append((void **)&array.children, &array.count, &array.capacity, (void *)&item, sizeof(jasoon_node_t *));
    }

    size_to_fit((void **)&array.children, array.count, sizeof(jasoon_node_t *));

    node->type = JASOON_ARRAY;
    node->array = array;

    parser->cursor++;
    return node;
}

jasoon_node_t *jasoon_parse_string_node(jasoon_token_t *token) {
    jasoon_node_t *node = malloc(sizeof(jasoon_node_t));
    jasoon_string_t string = {
        .string = token->value,
        .size = token->size,
    };

    node->type = JASOON_STRING;
    node->string = string;

    return node;
}

jasoon_node_t *jasoon_parse_number_node(jasoon_token_t *token) {
    jasoon_node_t *node = malloc(sizeof(jasoon_node_t));

    char num_char[token->size + 1];
    char *end_ptr;

    memcpy(num_char, token->value, token->size);
    num_char[token->size] = '\0';

    double num = strtod(num_char, &end_ptr);
    if (*end_ptr != '\0') {
        invalid(100);
    }

    node->type = JASOON_NUMBER;
    node->number = num;

    return node;
}

jasoon_node_t *jasoon_parse_literal_node(jasoon_node_type_t type) {
    jasoon_node_t *node = malloc(sizeof(jasoon_node_t));
    node->type = type;
    return node;
}

void jasoon_free_object(jasoon_object_t object, jasoon_buffer_t buffer) {
    for (int i = 0; i < object.count; i++) {
        jasoon_free_node(object.children[i].value, buffer);
    }
    free(object.children);
}

void jasoon_free_array(jasoon_array_t array, jasoon_buffer_t buffer) {
    for (int i = 0; i < array.count; i++) {
        jasoon_free_node(array.children[i], buffer);
    }
    free(array.children);
}

void jasoon_free_node(jasoon_node_t *node, jasoon_buffer_t buffer) {
    if (!node) {
        return;
    }

    switch (node->type) {
    case JASOON_ARRAY:
        jasoon_free_array(node->array, buffer);
        break;
    case JASOON_OBJECT:
        jasoon_free_object(node->object, buffer);
        break;
    case JASOON_STRING:
        if (node->string.size > 0) {
            char *string_start = node->string.string;
            char *string_end = node->string.string + (node->string.size - 1);

            if (!(string_end <= buffer.end && string_start >= buffer.start)) {
                free(node->string.string);
            }
        }

        break;
    default:
        break;
    }

    free(node);
}

void jasoon_free(jasoon_t json) {
    jasoon_free_node(json.node, json.parser.tokens.buffer);
    free(json.parser.tokens.buffer.start);
}
