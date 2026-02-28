#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_manager.h"

int file_save_ticket(const Ticket *ticket) {

    if (!ticket) return 0;

    // Obtener fecha actual para nombre del archivo
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (!t) return 0;

    char filename[128];

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

    fprintf(file, "Radicado: %s\n", ticket->radicado);
    fprintf(file, "Identificación: %s\n", ticket->identificacion);
    fprintf(file, "Correo: %s\n", ticket->correo);
    fprintf(file, "Tipo de reclamación: %s\n", ticket->tipo);

    fclose(file);

    return 1;
}