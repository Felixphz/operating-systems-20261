#ifndef INPUT_H
#define INPUT_H

char* input_read_line(const char *mensaje);

char* input_read_validated(const char *mensaje,
                           int (*validator)(const char *));

#endif