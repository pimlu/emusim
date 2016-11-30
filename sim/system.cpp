#include "system.h"

namespace sim {

System::System(int memory, int quantum, std::istream &in, std::ostream &out) : memory(memory), in(in), out(out) {
    sched = new Scheduler(this, quantum);
}

System::~System() {
    delete sched;
}

const int IOCOST = 30;
bool System::run(int c) {
    /*
enum Type {
    NONE, END, PRINT, INPUT,
    PRINTN, INPUTN, READ, WRITE,
    IORES
};
*/
    bool ret = false;
    cycles += c;
    while(cycles > 0) {
        if(blockQueue.empty()) {
            cycles = 0;
            return ret;
        }

        ProcCall req = blockQueue.front();
        Syscall *sc = req.second;
        ProcRes res(req.first, nullptr);
        //we use blocks inside cases to avoid jumping over initialization
        switch(req.second->type) {
            case Type::NONE:

                break;
            case Type::END:
                {SCInt *sci = (SCInt*) sc;
                if(cycles<5) return ret;
                cycles -= 5;

                usedMem -= res.first->memory;
                res.first = nullptr;
                //TODO: do something with return code

                }break;
            case Type::PRINT:
                {SCString *scs = (SCString*) sc;
                if(cycles<IOCOST) return ret;
                cycles -= IOCOST;

                out.write(scs->str, scs->len);
                res.second = new SRInt(Type::IORES, true);

                }break;
            case Type::INPUT:
                {if(cycles<IOCOST) return ret;
                cycles -= IOCOST;

                char *buffer = new char[256];
                in.getline(buffer, 256);

                res.second = new SRString(Type::IORES, buffer, in.gcount());

                }break;
            case Type::PRINTN:
                {SCInt *sci = (SCInt*) sc;
                if(cycles<IOCOST) return ret;
                cycles -= IOCOST;

                out << sci->val << std::endl;

                res.second = new SRInt(Type::IORES, 1);

                }break;
            case TYPE::INPUTN:
                {if(cycles<IOCOST) return ret;
                cycles -= IOCOST;

                int n;
                in >> n;

                res.second = new SRInt(Type::IORes, n);

                }break;
        }
        ret = true;
        blockQueue.pop();
        delete sc;
        if(res.first) finishQueue.push(res);
    }

    return ret;
}

}
