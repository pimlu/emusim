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
short* EmuProcess::convertToValue(int v, bool b, short* out)
{
    // register (A, B, C, X, Y, Z, I or J, in that order)
    if(0x00 <= v && v <= 0x07)
    {
        return &((short*) &registers)[v];
    }
    // [register]
    else if(0x08 <= v && v <= 0x0f)
    {
        unsigned short addr = ((short*)&registers)[v - 0x08];
        return &ram[addr];
    }
    // [register + next word]
    else if(0x10 <= v && v <= 0x17)
    {
        unsigned short addr = ((short*)&registers)[v - 0x08];
        addr += readNextWord();
        return &ram[addr];
    }
    // literal value 0xffff-0x1e (-1..30) (literal) (only for a)
    else if(0x20 <= v && v <= 0x3f && !b)
    {
        *out = (v - 0x21);
        return out;
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
                *out = registers.PC;
                return out;

            // EX
            case 0x1d:
                break;

            // [next word]
             case 0x1e:
                *out = ram[(unsigned short) readNextWord()];
                return out;

            // next word (literal)
            case 0x1f:
                *out = readNextWord();
                return out;
        }
    }
    return out;
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
    int opcode, a, b;

    short a_value = 0, b_value = 0;
    short *a_ptr, *b_ptr;

    for(int i = 0; i < c; i++)
    {
        word = readNextWord();

        printf("Word: \t\t 0x%hx\n", word);

        opcode = word & WORD_OPCODE_MASK;
        b = (word & WORD_B_MASK) >> WORD_OPCODE_LENGTH;
        a = (word & WORD_A_MASK) >> (WORD_B_LENGTH + WORD_OPCODE_LENGTH);

        // We have a special opcode
        if(opcode == 0)
        {
            opcode = b;
            b = 0;
        }

        printf("Opcode: \t 0x%hx \t a: 0x%hx \t b: 0x%hx \n", opcode, a, b);

        // Convert a and b to proper value using value table
        a_ptr = convertToValue(a, false, &a_value);
        b_ptr = convertToValue(b, true, &b_value);

        printf("Values: \t\t a: 0x%hx \t b: 0x%hx \t \n", *a_ptr, *b_ptr);

        // Handle the opcode
        switch(opcode)
        {
            case Opcodes::SET:
                printf("Instruction: \t SET\n");
                *b_ptr = *a_ptr;
                break;

            case Opcodes::ADD:
                printf("Instruction: \t ADD\n");
                *b_ptr += *a_ptr;
                break;

            case Opcodes::SUB:
                printf("Instruction: \t SUB\n");
                *b_ptr -= *a_ptr;
                break;

            case SpecialOpcodes::INT:
                printf("Instruction: \t INT\n");
                lastCall = static_cast<Type>(a);
                goto END;

            default:
                printf("Instruction: \t 0x%hx (unknowned)\n", opcode);
                break;
        }
        printRegisters();
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
