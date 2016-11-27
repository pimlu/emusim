#ifndef EMUPROCESS_H
#define EMUPROCESS_H

#include "../sim/process.h"

namespace emu {

class EmuProcess : public sim::Process
{
public:
    EmuProcess(char *ram, int len);
    char *ram;
};

}

#endif // EMUPROCESS_H
