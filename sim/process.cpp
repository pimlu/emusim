#include "process.h"

namespace sim {

Process::Process(int memory) : memory(memory) {}

DummyProcess::DummyProcess(int memory) : Process(memory) {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
Syscall* DummyProcess::run(int n, Sysres *res) {
    return new Syscall(Syscall::Type::NONE);
}
#pragma GCC diagnostic pop

}
