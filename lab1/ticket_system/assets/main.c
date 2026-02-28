#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "input.h"
#include "validator.h"
#include "ticket.h"

int main(void) {

    srand((unsigned int)time(NULL));

    char *id = input_read_validated(
        "Ingrese identificación: ",
        validate_numeric
    );

    if (!id) return 1;

    char *correo = input_read_validated(
        "Ingrese correo: ",
        validate_email
    );

    if (!correo) {
        free(id);
        return 1;
    }

    char *tipo = input_read_validated(
        "Ingrese tipo de reclamación: ",
        validate_not_empty
    );

    if (!tipo) {
        free(id);
        free(correo);
        return 1;
    }

    Ticket *ticket = ticket_create(id, correo, tipo);

    free(id);
    free(correo);
    free(tipo);

    if (!ticket) {
        printf("Error creando ticket\n");
        return 1;
    }

    if (ticket_save(ticket) == 0) {
        printf("\nRegistro exitoso.\n");
        printf("Radicado generado: %s\n", ticket->radicado);
    }

    ticket_destroy(ticket);

    return 0;
}