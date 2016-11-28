#include "emuprocess.h"

namespace emu {

using namespace sim;

EmuProcess::EmuProcess(char *ram, int len) : Process(len), ram(ram) {

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
Syscall* EmuProcess::run(int &c, Sysres *res) {
    switch(lastCall) {
        case Type::NONE:
            //foo
            break;
        case Type::READ:
            //bar
            break;
        default:
            //error, do an END syscall or something
            ;
    }

    //run up to c emulation cycles, counting c as a reference down to 0...

    //then finish with a syscall
    Syscall *ret = new Syscall(Type::NONE);

    //then make sure our state is set up for when we resume
    lastCall = ret->type;
    return ret;
}
#pragma GCC diagnostic pop

}
