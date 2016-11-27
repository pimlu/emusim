#ifndef EMUPROCESS_H
#define EMUPROCESS_H

#include "../sim/process.h"

namespace emu {

class EmuProcess : public sim::Process
{
public:
    EmuProcess(char *ram, int len);
    Syscall* run(int &c, Sysres *res) override;
    char *ram;
};

}

#endif // EMUPROCESS_H
