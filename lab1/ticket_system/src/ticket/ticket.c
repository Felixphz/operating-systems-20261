#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ticket.h"

static char* generar_radicado(void) {

    time_t t = time(NULL);
    int aleatorio = rand() % 10000;

    char *radicado = malloc(32);
    if (!radicado) return NULL;

    snprintf(radicado, 32, "%ld_%04d", t, aleatorio);

    return radicado;
}

Ticket* ticket_create(const char *id,
                      const char *correo,
                      const char *tipo) {

    Ticket *ticket = malloc(sizeof(Ticket));
    if (!ticket) return NULL;

    // Inicializar todos los punteros a NULL para evitar memory leaks
    ticket->identificacion = NULL;
    ticket->correo = NULL;
    ticket->tipo = NULL;
    ticket->radicado = NULL;

    ticket->identificacion = malloc(strlen(id) + 1);
    ticket->correo = malloc(strlen(correo) + 1);
    ticket->tipo = malloc(strlen(tipo) + 1);
    
    if (ticket->identificacion) strcpy(ticket->identificacion, id);
    if (ticket->correo) strcpy(ticket->correo, correo);
    if (ticket->tipo) strcpy(ticket->tipo, tipo);
    
    ticket->radicado = generar_radicado();

    if (!ticket->identificacion ||
        !ticket->correo ||
        !ticket->tipo ||
        !ticket->radicado) {

        ticket_destroy(ticket);
        return NULL;
    }

    return ticket;
}

int ticket_save(const Ticket *ticket) {

    char nombre[100];

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    strftime(nombre, sizeof(nombre),
             "assets/ticket_%Y%m%d%H%M.txt",
             tm_info);

    FILE *fp = fopen(nombre, "w");
    if (!fp) {
        perror("Error al crear archivo");
        return -1;
    }

    fprintf(fp, "Radicado: %s\n", ticket->radicado);
    fprintf(fp, "Identificación: %s\n", ticket->identificacion);
    fprintf(fp, "Correo: %s\n", ticket->correo);
    fprintf(fp, "Tipo de reclamación: %s\n", ticket->tipo);

    fclose(fp);

    return 0;
}

void ticket_destroy(Ticket *ticket) {
    if (!ticket) return;

    free(ticket->identificacion);
    free(ticket->correo);
    free(ticket->tipo);
    free(ticket->radicado);
    free(ticket);
}