/*
 * Emulator for DCPU-16
 * https://raw.githubusercontent.com/gatesphere/demi-16/master/docs/dcpu-specs/dcpu-1-7.txt
*/

#include <cstdio>
#include "emuprocess.h"

#define DEBUG_PRINT false

using namespace sim;

namespace emu {

EmuProcess::EmuProcess(char *ram, int len) : Process(len), ram((short*) ram) { }

// b => true, if finding value for 'b', otherwise we're finding value for 'a'
short* EmuProcess::convertToValue(int v, bool b, short* out)
{
    // register (A, B, C, X, Y, Z, I or J, in that order)
    if(0x00 <= v && v <= 0x07)
    {
        out = &((short*) &registers)[v];
        return out;
    }
    // [register]
    else if(0x08 <= v && v <= 0x0f)
    {
        unsigned short addr = ((short*)&registers)[v - 0x08];
        out = &ram[addr];
        return out;
    }
    // [register + next word]
    else if(0x10 <= v && v <= 0x17)
    {
        unsigned short addr = ((short*)&registers)[v - 0x10];
        addr += readNextWord();
        out = &ram[addr];
        return out;
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
                out = &registers.PC;
                return out;

            // EX
            case 0x1d:
                break;

            // [next word]
             case 0x1e:
                out = &ram[(unsigned short) readNextWord()];
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
    // Handle result from our last call
    switch(lastCall)
    {
        case NONE: break;

        case READ:      break;
        case WRITE:     break;

        case INPUT:     break;
        case INPUTN:    break;

        // Likely some kind of error, we will go ahead an terminate the process
        default: return new Syscall(Type::END);
    }

    int word;
    int opcode, a, b;

    short a_value = 0, b_value = 0;
    short *a_ptr, *b_ptr;

    Syscall *ret = new Syscall(Type::NONE);

    // Perform only C cycles or iterations (as specified by the scheduler)
    for(int i = 0; i < c; i++)
    {
        word = readNextWord();

        if(DEBUG_PRINT) printf("Word: \t\t 0x%hx\n", word);

        opcode = word & WORD_OPCODE_MASK;
        b = (word & WORD_B_MASK) >> (WORD_OPCODE_LENGTH);
        a = (word & WORD_A_MASK) >> (WORD_B_LENGTH + WORD_OPCODE_LENGTH);

        // We have a special opcode
        if(opcode == 0)
        {
            opcode = b;
            b = 0;
        }

        // Convert a and b to proper value using value table
        a_ptr = convertToValue(a, false, &a_value);
        b_ptr = convertToValue(b, true, &b_value);

        if(DEBUG_PRINT) printf("Opcode: \t 0x%hx \t a: 0x%hx \t b: 0x%hx \n", opcode, a, b);
        if(DEBUG_PRINT) printf("Values: \t\t a: 0x%hx \t b: 0x%hx \t \n", *a_ptr, *b_ptr);
        if(DEBUG_PRINT) printf("-----\n");

        if(!skip_instruction)
        {
            // Handle the opcode
            switch(opcode)
            {
                case Opcodes::SET: *b_ptr = *a_ptr;     break;
                case Opcodes::ADD: *b_ptr += *a_ptr;    break;
                case Opcodes::SUB: *b_ptr -= *a_ptr;    break;

                case Opcodes::MUL: *b_ptr *= *a_ptr;    break;
                case Opcodes::DIV: *b_ptr /= *a_ptr;    break;

                case Opcodes::AND: *b_ptr &= *a_ptr;    break;
                case Opcodes::BOR: *b_ptr |= *a_ptr;    break;
                case Opcodes::XOR: *b_ptr ^= *a_ptr;    break;

                case Opcodes::ASR: *b_ptr >>= *a_ptr;   break;
                case Opcodes::SHL: *b_ptr <<= *a_ptr;   break;

                case Opcodes::IFB: if(*a_ptr & *b_ptr == 0) skip_instruction = true; break;
                case Opcodes::IFC: if(*a_ptr & *b_ptr != 0) skip_instruction = true; break;

                case Opcodes::IFE: if(*a_ptr != *b_ptr) skip_instruction = true; break;
                case Opcodes::IFN: if(*a_ptr == *b_ptr) skip_instruction = true; break;

                case Opcodes::IFG: if(*a_ptr < *b_ptr) skip_instruction = true; break;
                case Opcodes::IFL: if(*a_ptr > *b_ptr) skip_instruction = true; break;

                case SpecialOpcodes::INT:
                {
                    Type type = static_cast<Type>(*a_ptr);

                    switch(type)
                    {
                        case PRINT:
                            // DCPU-16 is word aligned so we will just treat string as seperated by spacess...
                            // Its ultimately okay because it looks ~cool~ and ~retro~ that way.
                            ret = new SCString(Type::PRINT, (char*) (ram + registers.A), (registers.B * 2) - 1);
                            break;

                        case PRINTN:
                            ret = new SCInt(Type::PRINTN, registers.A);
                            break;

                        case READ:      break;
                        case WRITE:     break;

                        case INPUT:     ret = new Syscall(Type::INPUT);     break;
                        case INPUTN:    ret = new Syscall(Type::INPUTN);    break;
                        case END:       ret = new Syscall(Type::END);       break;

                        default:        ret = new Syscall(Type::NONE);      break;
                    }
                    goto END;
                }

                default:
                    printf("Instruction: \t 0x%hx (unknowned/unimplemented)\n", opcode);
                    break;
            }
        }
        else skip_instruction = false;

        if(DEBUG_PRINT) printRegisters();
        if(DEBUG_PRINT) printf("Local Cycle: \t %d\n\n", i + 1);
    }
    END:
    return ret;
}
}
