#include "scheduler.h"

#include <iostream>

namespace sim {

Scheduler::Scheduler(System *system, int memory, int quantum) :
    system(system), memory(memory), quantum(quantum) {

}

void Scheduler::doSim(int n, bool &paused) {
    cycle += n; //does nothing for now
    system->out << "Running "<< n << " cycles..." << std::endl;
    while(!paused) {
        int prevCycles = cyclesLeft;
        bool empty = waitQueue.empty();
        if(cyclesLeft <= 0) {
            cyclesLeft = quantum;
            if(!empty) {
                curProc = waitQueue.front();
                waitQueue.pop();
            }
        }
        //spend time running a process if we have one
        Process *proc = curProc.first;
        if(proc) {
            Syscall *call = proc->run(cyclesLeft, curProc.second);
            delete curProc.second;
            if(call->type == NONE) {
                waitQueue.push(ProcRes(proc, new Sysres(Type::NONE)));
            } else {
                system->blockQueue.push(ProcCall(proc, call));
            }
        } else cyclesLeft = 0;
        //run the kernel/system for the time we spent
        system->run(prevCycles - cyclesLeft);
        //check its results into the waiting queue
        while(!system->finishQueue.empty()) {
            waitQueue.push(system->finishQueue.front());
            system->finishQueue.pop();
        }
    }
}

void Scheduler::add(Process *p) {
    jobQueue.push(p);
}

}
