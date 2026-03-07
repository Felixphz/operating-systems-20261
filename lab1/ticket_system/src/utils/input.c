/*
 * input.c - Lectura y validación de entradas desde stdin
 *
 * Expone dos funciones: una para leer una línea de texto y otra que
 * repite la lectura hasta que el contenido pase una función validadora.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

/* Tamaño máximo de la línea leída desde stdin */
#define BUFFER_SIZE 256

/*
 * input_read_line - Lee una línea de texto desde stdin
 *
 * Muestra el mensaje indicado, lee hasta BUFFER_SIZE caracteres y
 * devuelve la cadena sin el salto de línea final en memoria dinámica.
 *
 * Parámetros:
 *   mensaje - Texto que se imprime antes de leer la entrada
 *
 * Retorna: puntero a la cadena leída (malloc), o NULL si falla la lectura
 *          o la asignación de memoria.
 */
char* input_read_line(const char *mensaje) {

    char buffer[BUFFER_SIZE];

    printf("%s", mensaje);

    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
        return NULL;
    }

    /* Eliminar el salto de línea que deja fgets al final del buffer */
    buffer[strcspn(buffer, "\n")] = '\0';

    /* Reservar memoria exacta para la cadena (sin espacio desperdiciado) */
    char *line = malloc(strlen(buffer) + 1);
    if (!line) return NULL;

    strcpy(line, buffer);

    return line;
}

/*
 * input_read_validated - Lee repetidamente hasta obtener una entrada válida
 *
 * Llama a input_read_line en bucle y aplica la función validadora a cada
 * resultado. Cuando la validación pasa, retorna la cadena al llamador.
 * La memoria de intentos fallidos se libera internamente.
 *
 * Parámetros:
 *   mensaje   - Texto que se muestra en cada intento de lectura
 *   validator - Puntero a función que retorna 1 si la entrada es válida,
 *               0 en caso contrario
 *
 * Retorna: puntero a la cadena válida (malloc), o NULL si falla stdin.
 */
char* input_read_validated(const char *mensaje,
                           int (*validator)(const char *)) {

    char *line = NULL;

    while (1) {

        /* Liberar el intento anterior antes de leer uno nuevo */
        if (line) {
            free(line);
            line = NULL;
        }

        line = input_read_line(mensaje);

        if (!line) {
            printf("Error leyendo entrada.\n");
            return NULL;
        }

        /* Si la entrada pasa la validación, devolverla al llamador */
        if (validator(line)) {
            return line;
        }

        printf("Entrada inválida. Intente nuevamente.\n");
    }
}
