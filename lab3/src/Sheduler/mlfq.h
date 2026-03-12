#ifndef MLFQ_H
#define MLFQ_H

#include "process/process.h"

void run_mlfq(
    Process processes[],
    int n,
    int boost_interval
);

#endif