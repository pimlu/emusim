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
    Process *curProc = nullptr; //current active process
    ProcCall curBlock;
    int cyclesLeft = 0; //cycles left before it context switches
public:
    Scheduler(System *system, int memory);
    int memory, used = 0;
    unsigned long long cycle = 0;
    //runs scheduler for a fixed number of cycles
    void doSim(int n);
    void add(sim::Process *p);
};

}

#endif // SCHEDULER_H
