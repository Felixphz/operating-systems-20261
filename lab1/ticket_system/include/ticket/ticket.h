#ifndef TICKET_H
#define TICKET_H

typedef struct {
    char *identificacion;
    char *correo;
    char *tipo;
    char *radicado;
} Ticket;

Ticket* ticket_create(const char *id,
                      const char *correo,
                      const char *tipo);

int ticket_save(const Ticket *ticket);

void ticket_destroy(Ticket *ticket);

#endif