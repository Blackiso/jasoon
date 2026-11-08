#include "lexer.h"
#include "helpers.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void match_keyword(char *start1, char *start2, size_t size) {
    if (memcmp(start1, start2, size) != 0) {
        invalid(10);
    }
}

jasoon_tokenizer_t jasoon_tokenize(char *buffer, size_t size) {
    size_t initial_size = 10;

    jasoon_tokenizer_t tokenizer = {
        .buffer = {.start = buffer, .end = buffer + size},
        .cursor = buffer,
        .tokens = malloc(initial_size * sizeof(jasoon_token_t)),
        .capacity = initial_size,
        .count = 0,
    };

    while (*tokenizer.cursor != '\0') {
        switch (*tokenizer.cursor) {
        case '{':
            jasoon_token_t l_brace = {
                .type = TOK_L_BRACE,
                .size = 1,
                .value = tokenizer.cursor,
            };
            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&l_brace, sizeof(jasoon_token_t));
            break;
        case '}':
            jasoon_token_t r_brace = {
                .type = TOK_R_BRACE,
                .size = 1,
                .value = tokenizer.cursor,
            };
            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&r_brace, sizeof(jasoon_token_t));
            break;
        case '[':
            jasoon_token_t l_bracket = {
                .type = TOK_L_BRACKET,
                .size = 1,
                .value = tokenizer.cursor,
            };
            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&l_bracket, sizeof(jasoon_token_t));
            break;
        case ']':
            jasoon_token_t r_bracket = {
                .type = TOK_R_BRACKET,
                .size = 1,
                .value = tokenizer.cursor,
            };
            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&r_bracket, sizeof(jasoon_token_t));
            break;
        case ':':
            jasoon_token_t colon = {
                .type = TOK_COLON,
                .size = 1,
                .value = tokenizer.cursor,
            };
            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&colon, sizeof(jasoon_token_t));
            break;
        case ',':
            jasoon_token_t comma = {
                .type = TOK_COMMA,
                .size = 1,
                .value = tokenizer.cursor,
            };
            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&comma, sizeof(jasoon_token_t));
            break;
        case '"':
            tokenizer.cursor++;

            char *string_start = tokenizer.cursor;
            size_t string_end = 0;

            while (*tokenizer.cursor != '"') {
                if (*tokenizer.cursor == '\n' || *tokenizer.cursor == '\0') {
                    perror("Malformed JSON string!");
                    exit(1);
                }
                string_end++;
                tokenizer.cursor++;
            }

            jasoon_token_t string = {
                .type = TOK_STRING,
                .size = string_end,
                .value = string_start,
            };

            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&string, sizeof(jasoon_token_t));
            break;
        case '0' ... '9':
        case '-':
            char *number_start = tokenizer.cursor;
            size_t number_end = 0;

            do {
                number_end++;
                tokenizer.cursor++;
            } while (isdigit(*tokenizer.cursor) || *tokenizer.cursor == '.');

            if (*tokenizer.cursor == 'e' || *tokenizer.cursor == 'E') {
                number_end++;
                tokenizer.cursor++;

                if (*tokenizer.cursor == '+' || *tokenizer.cursor == '-') {
                    number_end++;
                    tokenizer.cursor++;
                }

                while (isdigit(*tokenizer.cursor)) {
                    number_end++;
                    tokenizer.cursor++;
                }
            }

            jasoon_token_t number = {
                .type = TOK_NUMBER,
                .size = number_end,
                .value = number_start,
            };

            append((void **)&tokenizer.tokens, &(tokenizer.count), &(tokenizer.capacity), (void *)&number, sizeof(jasoon_token_t));
            tokenizer.cursor--;
            break;
        case 't':
            size_t true_keyword_size = 4;
            char *true_keyword = "true";
            match_keyword(tokenizer.cursor, true_keyword, true_keyword_size);

            jasoon_token_t true_token = {
                .type = TOK_TRUE,
                .size = true_keyword_size,
                .value = tokenizer.cursor,
            };

            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&true_token, sizeof(jasoon_token_t));
            tokenizer.cursor += (true_keyword_size - 1);

            break;
        case 'f':
            size_t false_keyword_size = 5;
            char *false_keyword = "false";
            match_keyword(tokenizer.cursor, false_keyword, false_keyword_size);

            jasoon_token_t false_token = {
                .type = TOK_FALSE,
                .size = false_keyword_size,
                .value = tokenizer.cursor,
            };

            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&false_token, sizeof(jasoon_token_t));
            tokenizer.cursor += (false_keyword_size - 1);

            break;
        case 'n':
            size_t null_keyword_size = 4;
            char *null_keyword = "null";
            match_keyword(tokenizer.cursor, null_keyword, null_keyword_size);

            jasoon_token_t null_token = {
                .type = TOK_NULL,
                .size = null_keyword_size,
                .value = tokenizer.cursor,
            };

            append((void **)&(tokenizer.tokens), &(tokenizer.count), &(tokenizer.capacity), (void *)&null_token, sizeof(jasoon_token_t));
            tokenizer.cursor += (null_keyword_size - 1);

            break;
        default:
            if (*tokenizer.cursor != '\n' && *tokenizer.cursor != ' ' && *tokenizer.cursor != '\t' && *tokenizer.cursor != '\r') {
                printf("Invalid charachter: (%c)", *tokenizer.cursor);
                invalid(20);
            }
            break;
        }

        tokenizer.cursor++;
    }

    size_to_fit((void **)&(tokenizer.tokens), tokenizer.count, sizeof(jasoon_token_t));
    return tokenizer;
}
