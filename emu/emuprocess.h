#ifndef EMUPROCESS_H
#define EMUPROCESS_H

#include "../sim/process.h"

namespace emu {

using namespace sim;

class EmuProcess : public Process
{
    static const int WORD_SIZE = 2;
    static const int BITS_PER_BYTE = 8;

    static const int  WORD_OPCODE_MASK = 0x1F;
    static const char   WORD_OPCODE_LENGTH = 5;

    static const int  WORD_B_MASK = WORD_OPCODE_MASK << WORD_OPCODE_LENGTH;
    static const char   WORD_B_LENGTH = 5;

    static const int  WORD_A_MASK = 0xFFFF & ~(WORD_OPCODE_MASK | WORD_B_MASK);
    static const char   WORD_A_LENGTH = 6;

    enum Opcodes
    {
        SET = 0x01,
        ADD = 0x02,
        SUB = 0x03,
        MUL = 0x04,
        MLI = 0x05,
        DIV = 0x06,
        DVI = 0x07,
        MOD = 0x08,
        MDI = 0x09,
        AND = 0x0A,
        BOR = 0x0B,
        XOR = 0x0C,
        SHR = 0x0D,
        ASR = 0x0E,
        SHL = 0x0F
    };

    enum SpecialOpcodes
    {
        INT = 0x08
    };

    struct Registers
    {
        short A, B, C, X, Y, Z, I, J;
        short PC, SP;
        short EX, IA;
    };

    public:
        EmuProcess(char *ram, int len);
        Syscall* run(int &c, Sysres *res) override;
        short *ram;
        Registers registers = {};
        Type lastCall = Type::NONE;
    };
}

#endif // EMUPROCESS_H
