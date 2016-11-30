#include "scheduler.h"

#include <iostream>

namespace sim {

Scheduler::Scheduler(System *system, int quantum) :
    system(system), quantum(quantum) {

}

unsigned long long Scheduler::doSim(unsigned long long n, bool &paused) {
    unsigned long long totalSim = 0;
    //system->out << "Running "<< n << " cycles..." << std::endl;
    while(!paused && totalSim < n) {
        while(!jobQueue.empty() && jobQueue.front()->memory <= system->memory - system->usedMem) {
            Process *proc = jobQueue.front();
            system->usedMem += proc->memory;
            waitQueue.push_back(ProcRes(proc, new Sysres(Type::NONE)));
            jobQueue.pop_front();
        }
        bool empty = waitQueue.empty();
        if(cyclesLeft <= 0) {
            cyclesLeft = quantum;
            if(!empty) {
                curProc = waitQueue.front();
                waitQueue.pop_front();
            }
        }
        int prevCycles = cyclesLeft;
        //spend time running a process if we have one
        Process *proc = curProc.first;
        if(proc) {
            Syscall *call = proc->run(cyclesLeft, curProc.second);
            delete curProc.second;
            if(call->type == NONE) {
                waitQueue.push_back(ProcRes(proc, new Sysres(Type::NONE)));
            } else {
                system->blockQueue.push_back(ProcCall(proc, call));
                curProc = ProcRes(nullptr, nullptr);
            }

        } else cyclesLeft = 0;
        int spent = prevCycles - cyclesLeft;
        pcbs[proc].cycles += spent;
        cyclesLeft = 0; //ignores how much they spend for now
        if(spent==0) system->out << "spent "<<spent<<std::endl;
        //run the kernel/system for the time we spent
        system->runSyscalls(spent);
        totalSim += spent;
        cycle += spent;
        //check its results into the waiting queue
        while(!system->finishQueue.empty()) {
            ProcRes res = system->finishQueue.front();
            if(res.second->type == Type::END) {
                pcbs.erase(res.first);
                delete res.first;
                memory -= res.first->memory;
                //TODO react to return code
            } else waitQueue.push_back(res);
            system->finishQueue.pop_front();
        }
    }
    return totalSim;
}

int Scheduler::add(Process *p, std::string name) {
    jobQueue.push_back(p);
    pcbs[p] = PCB();
    pcbs[p].name = name;
    return pcbs[p].pid = curpid++;
}

template <typename T, typename V>
void clearDeque(deque<T> q, V v) {
    q.erase(std::remove_if(q.begin(), q.end(), [&](T e) {
                return e.first == v;
            }), q.end());
}

void Scheduler::remove(Process *p) {
    jobQueue.erase(std::remove(jobQueue.begin(), jobQueue.end(), p),
                   jobQueue.end());
    clearDeque(waitQueue, p);
    clearDeque(system->blockQueue, p);
    clearDeque(system->finishQueue, p);
}
void Scheduler::remove(int pid) {
    remove(find(pid));
}

Process* Scheduler::find(int pid) {
    for(auto i : pcbs) {
        if(i.second.pid == pid) return i.first;
    }
    return nullptr;
}

}
