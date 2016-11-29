#include "process.h"

namespace sim {

Process::Process(int memory) : memory(memory) {}

DummyProcess::DummyProcess(int memory) : Process(memory) {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
Syscall* DummyProcess::run(int &c, Sysres *res) {
    c=0;
    static bool foo = true;
    return (foo = !foo) ?
                new Syscall(Type::NONE) :
                new SCString(Type::PRINT, "foo\n", 4);
}
#pragma GCC diagnostic pop

}
