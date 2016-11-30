#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <deque>

namespace sim { class System; }
#include "scheduler.h"
#include "process.h"

namespace sim {

using std::deque;

class System {
public:
    System(int memory, int quantum, std::istream &in, std::ostream &out);
    ~System();
    int memory, usedMem=0;
    std::istream &in;
    std::ostream &out;
    deque<ProcCall> blockQueue; //blocked - syscall result (usually IO)
    deque<ProcRes> finishQueue; //processes ready to be put in the waiting queue
    bool run(int c); //passes time for the system, returns true if an entry finished
    int cycles = 0; //how far along in the blockQueue we are
    Scheduler *sched;
};


}

#endif // SYSTEM_H
