#include "emuprocess.h"

namespace emu {

EmuProcess::EmuProcess(char *ram, int len) : Process(len), ram(ram) {

}

}
