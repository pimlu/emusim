#ifndef EMUPROCESS_H
#define EMUPROCESS_H

#include "../sim/process.h"

namespace emu {

using namespace sim;

class EmuProcess : public Process
{
public:
    EmuProcess(char *ram, int len);
    Syscall* run(int &c, Sysres *res) override;
    char *ram;
    Type lastCall = Type::NONE;
};

}

#endif // EMUPROCESS_H
