#include "scheduler.h"

#include <iostream>

namespace sim {

Scheduler::Scheduler(System *system, int memory) :
    system(system), memory(memory) {

}

void Scheduler::doSim(int n) {
    cycle += n; //does nothing for now
    std::cout << "Running "<< n << " cycles..." << std::endl;
}

void Scheduler::add(Process *p) {
    jobQueue.push(p);
}

}
