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

// b => true, if finding value for 'b', otherwise we're finding value for 'a'
int EmuProcess::convertToValue(int v, bool b)
{
    // register (A, B, C, X, Y, Z, I or J, in that order)
    if(0x00 <= v && v <= 0x07)
    {
        return (int) ((short*) &registers)[v];
    }
    // [register]
    else if(0x08 <= v && v <= 0x0f)
    {
        int addr = (int) ((short*) &registers)[v - 0x08];
        return (int) ram[addr];
    }
    // [register + next word]
    else if(0x10 <= v && v <= 0x17)
    {
        int addr = (int)((short*)&registers)[v - 0x10];
        addr += readNextWord();
        return (int) ram[addr];
    }
    // literal value 0xffff-0x1e (-1..30) (literal) (only for a)
    else if(0x20 <= v && v <= 0x3f && !b)
    {
        return (int) (v - 0x21);
    }
    else
    {
        switch(v)
        {
            // (PUSH / [--SP]) if in b, or (POP / [SP++]) if in a
            case 0x18:
                break;

            // [SP] / PEEK
            case 0x19:
                break;

            // [SP + next word] / PICK n
            case 0x1a:
                break;

            // SP
            case 0x1b:
                break;

            // PC
            case 0x1c:
                return registers.PC;

            // EX
            case 0x1d:
                break;

            // [next word]
             case 0x1e:
                return ram[(unsigned short) readNextWord()];

            // next word (literal)
            case 0x1f:
                return readNextWord();
        }
    }
    return 0;
}

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
        word = readNextWord();

        printf("Word: 0x%hx\n", word);

        opcode = word & WORD_OPCODE_MASK;
        b = (word & WORD_B_MASK) >> WORD_OPCODE_LENGTH;
        a = (word & WORD_A_MASK) >> (WORD_B_LENGTH + WORD_OPCODE_LENGTH);

        // We have a special opcode
        if(opcode == 0)
        {
            opcode = b;
            b = 0;
        }

        printf("Opcode: 0x%hx, a: 0x%hx, b: 0x%hx, \n", opcode, a, b);

        // Convert a and b to proper value using value table
        a = convertToValue(a, false);
        b = convertToValue(b, true);

         printf("Converted values: a: 0x%hx, b: 0x%hx, \n", opcode, a, b);

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
