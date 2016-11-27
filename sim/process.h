#ifndef PROCESS_H
#define PROCESS_H

#include <utility>

namespace sim { class Process; class DummyProcess; }
#include "syscalls.h"

namespace sim {

using ProcCall = std::pair<Process*, Syscall*>;

class Process {
public:
    Process(int memory);
    virtual ~Process() {}
    virtual Syscall* run(int &c, Sysres *res) = 0;
    int memory;
};

class DummyProcess : public Process {
public:
    DummyProcess(int memory);
    Syscall* run(int &c, Sysres *res) override;
};

}

#endif // PROCESS_H
