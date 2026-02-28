#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

#define BUFFER_SIZE 256

char* input_read_line(const char *mensaje) {

    char buffer[BUFFER_SIZE];

    printf("%s", mensaje);

    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
        return NULL;
    }

    // eliminar salto de línea
    buffer[strcspn(buffer, "\n")] = '\0';

    // reservar memoria exacta
    char *line = malloc(strlen(buffer) + 1);
    if (!line) return NULL;

    strcpy(line, buffer);

    return line;
}


char* input_read_validated(const char *mensaje,
                           int (*validator)(const char *)) {

    char *line = NULL;

    while (1) {

        if (line) {
            free(line);
            line = NULL;
        }

        line = input_read_line(mensaje);

        if (!line) {
            printf("Error leyendo entrada.\n");
            return NULL;
        }

        if (validator(line)) {
            return line;
        }

        printf("Entrada inválida. Intente nuevamente.\n");
    }
}
