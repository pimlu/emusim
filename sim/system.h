#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>

namespace sim { class System; }
#include "scheduler.h"
#include "process.h"

namespace sim {

class System {
public:
    System(int memory, std::istream &in, std::ostream &out);
    ~System();
    std::istream &in;
    std::ostream &out;
    std::queue<ProcCall> blockQueue; //blocked - syscall result (usually IO)
    bool pollBlocked(int c); //passes time for the system, returns true if an entry finished
    Sysres *getRes(); //gets syscall result for finished blocked ProcCall (null if !pollBlocked)
    int spentCycles = 0; //how far along in the blockQueue we are
    Scheduler *sched;
};


}

#endif // SYSTEM_H
