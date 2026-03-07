/*
 * ticket.c - Implementación del ciclo de vida de un Ticket
 *
 * Gestiona la creación, el guardado en disco y la liberación de memoria
 * de la estructura Ticket.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ticket.h"

/*
 * generar_radicado - Genera un número de radicado único
 *
 * Combina el timestamp Unix actual con un número aleatorio de 4 dígitos
 * para producir un identificador de la forma "<timestamp>_<XXXX>".
 *
 * Retorna: puntero a cadena asignada con malloc, o NULL si falla.
 */
static char* generar_radicado(void) {

    time_t t = time(NULL);
    int aleatorio = rand() % 10000;

    char *radicado = malloc(32);
    if (!radicado) return NULL;

    /* Formato: <unix_timestamp>_<número aleatorio de 4 dígitos> */
    snprintf(radicado, 32, "%ld_%04d", t, aleatorio);

    return radicado;
}

/*
 * ticket_create - Aloja y rellena un nuevo Ticket
 *
 * Copia cada campo en memoria dinámica independiente y genera el radicado.
 * Si cualquier asignación falla, libera todo y retorna NULL.
 *
 * Parámetros:
 *   id     - Identificación numérica del usuario
 *   correo - Correo electrónico del usuario
 *   tipo   - Tipo de reclamación
 *
 * Retorna: puntero al Ticket creado, o NULL en caso de error.
 */
Ticket* ticket_create(const char *id,
                      const char *correo,
                      const char *tipo) {

    Ticket *ticket = malloc(sizeof(Ticket));
    if (!ticket) return NULL;

    /* Inicializar todos los punteros a NULL para evitar memory leaks
     * en caso de fallo parcial durante la asignación de campos */
    ticket->identificacion = NULL;
    ticket->correo         = NULL;
    ticket->tipo           = NULL;
    ticket->radicado       = NULL;

    /* Asignar memoria para cada campo y copiar el contenido */
    ticket->identificacion = malloc(strlen(id) + 1);
    ticket->correo         = malloc(strlen(correo) + 1);
    ticket->tipo           = malloc(strlen(tipo) + 1);

    if (ticket->identificacion) strcpy(ticket->identificacion, id);
    if (ticket->correo)         strcpy(ticket->correo, correo);
    if (ticket->tipo)           strcpy(ticket->tipo, tipo);

    /* Generar el número de radicado único */
    ticket->radicado = generar_radicado();

    /* Verificar que todos los campos se asignaron correctamente */
    if (!ticket->identificacion ||
        !ticket->correo         ||
        !ticket->tipo           ||
        !ticket->radicado) {

        ticket_destroy(ticket);
        return NULL;
    }

    return ticket;
}

/*
 * ticket_save - Persiste el ticket en un archivo de texto
 *
 * Genera el nombre del archivo usando la fecha y hora actuales con el
 * formato "assets/ticket_YYYYMMDDHHMM.txt" y escribe todos los campos.
 *
 * Retorna: 0 si el guardado fue exitoso, -1 si hubo un error de E/S.
 */
int ticket_save(const Ticket *ticket) {

    char nombre[100];

    /* Obtener la hora local para construir el nombre del archivo */
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

    /* Escribir todos los campos del ticket en el archivo */
    fprintf(fp, "Radicado: %s\n",            ticket->radicado);
    fprintf(fp, "Identificación: %s\n",       ticket->identificacion);
    fprintf(fp, "Correo: %s\n",              ticket->correo);
    fprintf(fp, "Tipo de reclamación: %s\n", ticket->tipo);

    fclose(fp);

    return 0;
}

/*
 * ticket_destroy - Libera toda la memoria asociada a un Ticket
 *
 * Es seguro llamar esta función con un puntero NULL.
 */
void ticket_destroy(Ticket *ticket) {
    if (!ticket) return;

    free(ticket->identificacion);
    free(ticket->correo);
    free(ticket->tipo);
    free(ticket->radicado);
    free(ticket);
}
