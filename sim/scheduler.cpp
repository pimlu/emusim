#include "scheduler.h"

namespace sim {

Scheduler::Scheduler(System *system, int memory) :
    system(system), memory(memory) {

}

void Scheduler::run(int n) {
    cycle += n; //does nothing for now
}

void Scheduler::add(Process *p) {
    jobQueue.push(p);
}

}
