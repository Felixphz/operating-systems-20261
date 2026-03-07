/*
 * file_manager.c - Utilidad alternativa para guardar tickets en disco
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_manager.h"

/*
 * file_save_ticket - Guarda los datos de un ticket en un archivo de texto
 *
 * El nombre del archivo se forma con la fecha y hora completas, incluyendo
 * segundos, usando el formato "assets/ticket_YYYYMMDDHHMMSS.txt".
 *
 * Parámetros:
 *   ticket - Puntero al Ticket que se desea guardar (no debe ser NULL)
 *
 * Retorna: 1 si el guardado fue exitoso, 0 en caso de error.
 */
int file_save_ticket(const Ticket *ticket) {

    if (!ticket) return 0;

    /* Obtener la fecha y hora actuales para el nombre del archivo */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (!t) return 0;

    char filename[128];

    /* Construir el nombre con año, mes, día, hora, minuto y segundo */
    snprintf(filename, sizeof(filename),
             "assets/ticket_%04d%02d%02d%02d%02d%02d.txt",
             t->tm_year + 1900,
             t->tm_mon + 1,
             t->tm_mday,
             t->tm_hour,
             t->tm_min,
             t->tm_sec);

    FILE *file = fopen(filename, "w");

    if (!file) {
        perror("Error al crear el archivo");
        return 0;
    }

    /* Escribir los campos del ticket en el archivo */
    fprintf(file, "Radicado: %s\n",            ticket->radicado);
    fprintf(file, "Identificación: %s\n",       ticket->identificacion);
    fprintf(file, "Correo: %s\n",              ticket->correo);
    fprintf(file, "Tipo de reclamación: %s\n", ticket->tipo);

    fclose(file);

    return 1;
}
