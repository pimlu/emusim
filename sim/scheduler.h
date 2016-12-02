#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <deque>
#include <unordered_map>

namespace sim { class Scheduler; }
#include "system.h"
#include "process.h"

namespace sim {

struct PCB {
    unsigned long long cycles, sample, ioreqs, ibytes, obytes;
    std::string name;
    int pid, priority;
};

using std::deque;
using std::unordered_map;


class Scheduler {
    System *system;
public:
    Scheduler(System *system, int quantum);
    unordered_map<Process*, PCB> pcbs;

    deque<Process*> jobQueue; //waiting for adequate memory to start process
    deque<ProcRes> waitQueue; //waiting for cpu turn
    ProcRes curProc; //current active process
    int cyclesLeft = 0; //cycles left before it context switches

    int quantum, curpid = 1;
    unsigned long long cycle = 0;
    //runs scheduler for a fixed number of cycles
    int doSim(int n, bool &paused);
    int add(sim::Process *p, std::string name);
    void remove(sim::Process *p);
    void remove(int pid);
    Process* find(int pid);
};

}

#endif // SCHEDULER_H
