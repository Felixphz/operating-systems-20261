#ifndef PROCESS_H
#define PROCESS_H

typedef struct {

    int PID;

    int arrival_time;
    int burst_time;
    int remaining_time;

    int start_time;
    int finish_time;

    int first_response_time;

    int current_queue;

} Process;


/* Constructor del proceso */
Process create_process(int pid, int arrival, int burst);

/* Función auxiliar para imprimir procesos */
void print_process(Process p);

#endif