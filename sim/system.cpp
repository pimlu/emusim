#include "system.h"

namespace sim {

System::System(int memory, int quantum, std::istream &in, std::ostream &out) : in(in), out(out) {
    sched = new Scheduler(this, memory, quantum);
}

System::~System() {
    delete sched;
}

bool System::run(int c) {
    static int costs[] = {0};
    if(blockQueue.empty()) {
        spentCycles = 0;
        return false;
    }
    ProcCall req = blockQueue.front();

    return false;
}

}
