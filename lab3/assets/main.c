#include "process/process.h"
#include "Sheduler/mlfq.h"

int main()
{

    Process processes[4];

    processes[0] = create_process(1,0,8);
    processes[1] = create_process(2,1,4);
    processes[2] = create_process(3,2,9);
    processes[3] = create_process(4,3,5);

    int priority_boost =20;

    run_mlfq(processes,4,priority_boost);

    return 0;
}