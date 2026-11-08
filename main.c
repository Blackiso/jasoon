#include "helpers.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *json_file = fopen("test.json", "r");

    if (json_file == NULL) {
        printf("Failed to open file.");
        return 1;
    }

    fseek(json_file, 0, SEEK_END);
    size_t size = (size_t)ftell(json_file);
    rewind(json_file);

    char *buffer = malloc(size + 1);
    size_t bytes_read = fread(buffer, 1, size, json_file);
    buffer[bytes_read] = '\0';

    jasoon_t json = jasoon_parse(buffer, size);
    jasoon_print_node(json.node, 0);
    jasoon_free(json);

    return 0;
}
