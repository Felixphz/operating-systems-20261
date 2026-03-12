#include <stddef.h>
#include "Sheduler/mlfq.h"
#include "Sheduler/queue.h"
#include "utils/file_manager.h"

#define Q0_QUANTUM 1
#define Q1_QUANTUM 4
#define Q2_QUANTUM 8

void run_mlfq(Process processes[], int n, int boost_interval)
{

    Queue Q0, Q1, Q2;

    init_queue(&Q0);
    init_queue(&Q1);
    init_queue(&Q2);

    int time = 0;
    int completed = 0;

    while(completed < n)
    {

        /* agregar procesos que llegan */

        for(int i=0;i<n;i++)
        {
            if(processes[i].arrival_time == time)
            {
                enqueue(&Q0, &processes[i]);
            }
        }

        /* priority boost */

        if(time > 0 && time % boost_interval == 0)
        {
            while(!is_empty(&Q1))
            {
                Process *p = dequeue(&Q1);
                if(p != NULL)
                {
                    p->current_queue = 0;
                    enqueue(&Q0,p);
                }
            }

            while(!is_empty(&Q2))
            {
                Process *p = dequeue(&Q2);
                if(p != NULL)
                {
                    p->current_queue = 0;
                    enqueue(&Q0,p);
                }
            }
        }

        Queue *current_queue = NULL;
        int quantum = 0;

        if(!is_empty(&Q0))
        {
            current_queue = &Q0;
            quantum = Q0_QUANTUM;
        }
        else if(!is_empty(&Q1))
        {
            current_queue = &Q1;
            quantum = Q1_QUANTUM;
        }
        else if(!is_empty(&Q2))
        {
            current_queue = &Q2;
            quantum = Q2_QUANTUM;
        }
        else
        {
            time++;
            continue;
        }

        Process *p = dequeue(current_queue);
        
        if(p == NULL) continue;  /* Verificación de seguridad */

        if(p->start_time == -1)
        {
            p->start_time = time;
            p->first_response_time = time;
        }

        int executed = 0;

        while(executed < quantum && p->remaining_time > 0)
        {
            p->remaining_time--;

            time++;
            executed++;

            /* verificar nuevas llegadas */

            for(int i=0;i<n;i++)
            {
                if(processes[i].arrival_time == time)
                {
                    enqueue(&Q0, &processes[i]);
                }
            }

            if(p->remaining_time == 0)
            {
                p->finish_time = time;
                completed++;
                break;
            }
        }

        if(p->remaining_time > 0)
        {
            if(executed == quantum)
            {
                if(p->current_queue == 0)
                {
                    p->current_queue = 1;
                    enqueue(&Q1,p);
                }
                else if(p->current_queue == 1)
                {
                    p->current_queue = 2;
                    enqueue(&Q2,p);
                }
                else
                {
                    enqueue(&Q2,p);
                }
            }
            else
            {
                enqueue(current_queue,p);
            }
        }
    }

    /* Exportar resultados a CSV */
    export_results(processes, n, "mlfq_results.csv");
}