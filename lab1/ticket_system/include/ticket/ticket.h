#ifndef TICKET_H
#define TICKET_H

typedef struct {
    char *identificacion; /* Número de identificación del usuario        */
    char *correo;         /* Correo electrónico de contacto              */
    char *tipo;           /* Tipo o motivo de la reclamación             */
    char *radicado;       /* Radicado generado (timestamp + aleatorio)   */
} Ticket;

Ticket* ticket_create(const char *id,
                      const char *correo,
                      const char *tipo);

int ticket_save(const Ticket *ticket);

void ticket_destroy(Ticket *ticket);

#endif
