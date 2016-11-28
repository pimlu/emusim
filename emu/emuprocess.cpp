#include "emuprocess.h"

/*
 * Emulator for DCPU-16
 * https://raw.githubusercontent.com/gatesphere/demi-16/master/docs/dcpu-specs/dcpu-1-7.txt
*/

namespace emu {

using namespace sim;

EmuProcess::EmuProcess(char *ram, int len) : Process(len), ram((short*) ram) {

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

Syscall* EmuProcess::run(int &c, Sysres *res)
{
    switch(lastCall)
    {
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
    int word;
    int opcode, a, b, value;

    for(int i = 0; i < c; i++)
    {
        word = ram[registers.PC++];

        printf("Word: %04X\n", (int) (word & 0xFFFF));

        opcode = word & WORD_OPCODE_MASK;
        b = (word & WORD_B_MASK) >> WORD_OPCODE_LENGTH;
        a = (word & WORD_A_MASK) >> (WORD_B_LENGTH + WORD_OPCODE_LENGTH);

        // We have a special opcode
        if(opcode == 0)
        {
            opcode = b;
            a = b;
            b = 0;
        }

        printf("Opcode: 0x%X, a: 0x%X, b: 0x%X\n", opcode, a, b);

        // TODO: convert a and b to proper value using value table

        // Handle the opcode
        switch(opcode)
        {
            case Opcodes::ADD:
                printf("Performing ADD operation\n");
                break;

            case SpecialOpcodes::INT:
                printf("Performing INT operation\n");
                lastCall = static_cast<Type>(a);
                goto END;

            default:
                printf("Unimplemeted opcode: %04X\n", opcode);
                break;
        }
        printf("\n");
    }

    END:
    //then finish with a syscall
    Syscall *ret = new Syscall(lastCall);

    //then make sure our state is set up for when we resume
    //lastCall = ret->type;

    return ret;
}

#pragma GCC diagnostic pop

}
