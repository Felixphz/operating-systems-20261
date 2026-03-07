/*
 * main.c - Punto de entrada del sistema de tickets
 *
 * Solicita al usuario su identificación, correo y tipo de reclamación,
 * valida cada campo, crea un ticket y lo persiste en disco.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "input.h"
#include "validator.h"
#include "ticket.h"

int main(void) {

    /* Inicializar la semilla del generador de números aleatorios con el
     * tiempo actual para que el radicado sea único en cada ejecución */
    srand((unsigned int)time(NULL));

    /* Leer y validar la identificación numérica del usuario */
    char *id = input_read_validated(
        "Ingrese identificación: ",
        validate_numeric
    );

    if (!id) return 1;

    /* Leer y validar el correo electrónico (debe contener '@') */
    char *correo = input_read_validated(
        "Ingrese correo: ",
        validate_email
    );

    if (!correo) {
        free(id);
        return 1;
    }

    /* Leer y validar el tipo de reclamación (campo no vacío) */
    char *tipo = input_read_validated(
        "Ingrese tipo de reclamación: ",
        validate_not_empty
    );

    if (!tipo) {
        free(id);
        free(correo);
        return 1;
    }

    /* Crear la estructura Ticket con los datos ingresados */
    Ticket *ticket = ticket_create(id, correo, tipo);

    /* Liberar las cadenas temporales; ticket_create ya copió los datos */
    free(id);
    free(correo);
    free(tipo);

    if (!ticket) {
        printf("Error creando ticket\n");
        return 1;
    }

    /* Guardar el ticket en un archivo .txt dentro de assets/ */
    if (ticket_save(ticket) == 0) {
        printf("\nRegistro exitoso.\n");
        printf("Radicado generado: %s\n", ticket->radicado);
    }

    /* Liberar toda la memoria asociada al ticket */
    ticket_destroy(ticket);

    return 0;
}
