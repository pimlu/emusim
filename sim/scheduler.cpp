#include "scheduler.h"

#include <iostream>

namespace sim {

Scheduler::Scheduler(System *system, int memory, int quantum) :
    system(system), memory(memory), quantum(quantum) {

}

void Scheduler::doSim(int n, bool &paused) {
    int totalSim = 0;
    system->out << "Running "<< n << " cycles..." << std::endl;
    while(!paused && totalSim < n) {
        bool empty = waitQueue.empty();
        if(cyclesLeft <= 0) {
            cyclesLeft = quantum;
            if(!empty) {
                curProc = waitQueue.front();
                waitQueue.pop();
            }
        }
        int prevCycles = cyclesLeft;
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
        int spent = prevCycles - cyclesLeft;
        system->out << "spent "<<spent<<std::endl;
        //run the kernel/system for the time we spent
        system->run(spent);
        totalSim += spent;
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
