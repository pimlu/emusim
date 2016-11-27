#include "system.h"

namespace sim {

System::System(int memory, std::istream &in, std::ostream &out) : in(in), out(out) {
    sched = new Scheduler(this, memory);
}

System::~System() {
    delete sched;
}

bool System::pollBlocked(int c) {
    spentCycles += c;
    return false;
}

Sysres* System::getRes() {
    return nullptr;
}

}
