#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>


namespace sim { class Scheduler; }
#include "system.h"
#include "process.h"

namespace sim {

using std::queue;

class Scheduler {
    System *system;
    queue<Process*> jobQueue; //waiting for adequate memory to start process
    queue<ProcRes> waitQueue; //waiting for cpu turn
    ProcRes curProc; //current active process
    int cyclesLeft = 0; //cycles left before it context switches
public:
    Scheduler(System *system, int quantum);
    int memory, used = 0, quantum;
    unsigned long long cycle = 0;
    //runs scheduler for a fixed number of cycles
    void doSim(int n, bool &paused);
    void add(sim::Process *p);
};

}

#endif // SCHEDULER_H
