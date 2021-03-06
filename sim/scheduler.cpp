#include "scheduler.h"

#include <iostream>
#include <algorithm>
#include <sstream>

namespace sim {

Scheduler::Scheduler(System *system, int quantum) :
    system(system), quantum(quantum) {

}

int Scheduler::doSim(int n, bool &paused) {
    int totalSim = 0;
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
        int prevCycles = std::min(cyclesLeft, n-totalSim),
                runCycles = prevCycles;
        //spend time running a process if we have one
        Process *proc = curProc.first;
        if(proc) {
            Syscall *call = proc->run(runCycles, curProc.second);
            delete curProc.second;
            if(call->type == NONE) {
                waitQueue.push_back(ProcRes(proc, new Sysres(Type::NONE)));
            } else {
                system->addBlock(ProcCall(proc, call));
            }
            curProc = ProcRes(nullptr, nullptr);

        } else cyclesLeft = runCycles = 0;
        int spent = prevCycles - runCycles;
        cyclesLeft -= spent;
        pcbs[proc].cycles += spent;
        pcbs[proc].sample += spent;
        //cyclesLeft = 0; //ignores how much they spend for now
        //if(spent==0) system->out << "spent "<<spent<<std::endl;
        //run the kernel/system for the time we spent
        system->runSyscalls(spent);
        totalSim += spent;
        cycle += spent;
        //check its results into the waiting queue
        while(!system->finishQueue.empty()) {
            ProcRes res = system->finishQueue.front();
            if(res.second->type == Type::END) {
                std::stringstream out;
                out << "A process called " << pcbs[res.first].name
                            << " with pid " << pcbs[res.first].pid
                            << " just terminated after " << pcbs[res.first].cycles
                            << " cycles with return code "
                            << ((SRInt*) res.second)->val << "." <<std::endl;
                system->log(out.str().c_str());
                pcbs.erase(res.first);
                system->usedMem -= res.first->memory;
                delete res.first;
                //TODO react to return code
            } else waitQueue.push_back(res);
            system->finishQueue.pop_front();
        }
    }
    return totalSim;
}

int Scheduler::add(Process *p, std::string name, int priority) {
    jobQueue.push_back(p);
    pcbs[p] = PCB {};
    pcbs[p].name = name;
    pcbs[p].priority = priority;
    return pcbs[p].pid = curpid++;
}

template <typename T, typename V>
void clearDeque(deque<T> q, V v) {
    q.erase(std::remove_if(q.begin(), q.end(), [&](T e) {
                return e.first == v;
            }), q.end());
}

void Scheduler::remove(Process *p) {
    if(!p) return;
    jobQueue.erase(std::remove(jobQueue.begin(), jobQueue.end(), p),
                   jobQueue.end());
    clearDeque(waitQueue, p);
    system->blockQueue.remove(p);
    clearDeque(system->finishQueue, p);
    if(curProc.first == p) {
        delete curProc.second;
        curProc = {nullptr, nullptr};
    }
    system->finishQueue.push_back(ProcRes(p, new SRInt(Type::END, -1)));
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
