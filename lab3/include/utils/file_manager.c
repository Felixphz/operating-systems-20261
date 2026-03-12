#include <stdio.h>
#include "process/process.h"

void export_results(Process processes[], int n, const char *filename)
{

    FILE *f = fopen(filename,"w");
    
    if(f == NULL)
    {
        fprintf(stderr, "Error: No se pudo crear el archivo %s\n", filename);
        return;
    }

    fprintf(f,"PID,Arrival,Burst,Start,Finish,Response,Turnaround,Waiting\n");

    for(int i=0;i<n;i++)
    {

        int turnaround = processes[i].finish_time - processes[i].arrival_time;

        int response = processes[i].first_response_time - processes[i].arrival_time;

        int waiting = turnaround - processes[i].burst_time;

        fprintf(f,"%d,%d,%d,%d,%d,%d,%d,%d\n",
        processes[i].PID,
        processes[i].arrival_time,
        processes[i].burst_time,
        processes[i].start_time,
        processes[i].finish_time,
        response,
        turnaround,
        waiting);

    }

    fclose(f);
}