#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <deque>
#include <unordered_map>

namespace sim { class Scheduler; }
#include "system.h"
#include "process.h"

namespace sim {

struct PCB {
    unsigned long long cycles, ioreqs, ibytes, obytes;
};

using std::deque;
using std::unordered_map;

class Scheduler {
    System *system;
    unordered_map<Process*, PCB> pcbs;
    deque<Process*> jobQueue; //waiting for adequate memory to start process
    deque<ProcRes> waitQueue; //waiting for cpu turn
    ProcRes curProc; //current active process
    int cyclesLeft = 0; //cycles left before it context switches
public:
    Scheduler(System *system, int quantum);
    int memory, used = 0, quantum;
    unsigned long long cycle = 0;
    //runs scheduler for a fixed number of cycles
    void doSim(int n, bool &paused);
    void add(sim::Process *p);
    void remove(sim::Process *p);
};

}

#endif // SCHEDULER_H
