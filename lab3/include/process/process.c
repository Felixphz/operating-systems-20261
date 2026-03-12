#include "process/process.h"

Process create_process(int pid, int arrival, int burst)
{
    Process p;

    p.PID = pid;
    p.arrival_time = arrival;
    p.burst_time = burst;

    p.remaining_time = burst;

    p.start_time = -1;
    p.finish_time = -1;

    p.first_response_time = -1;

    p.current_queue = 0;

    return p;
}