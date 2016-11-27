#include "emuprocess.h"

namespace emu {

EmuProcess::EmuProcess(char *ram, int len) : Process(len), ram(ram) {

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
Syscall* EmuProcess::run(int n, Sysres *res) {
    return new Syscall(Syscall::Type::NONE);
}
#pragma GCC diagnostic pop

}
