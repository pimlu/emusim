#include "system.h"

namespace sim {

System::System(int memory, int quantum, std::istream &in, std::ostream &out) : memory(memory), in(in), out(out) {
    sched = new Scheduler(this, quantum);
}

System::~System() {
    delete sched;
}

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
                if(cycles<scs->len) return ret;
                cycles -= scs->len;

                out.write(scs->str, scs->len);
                res.second = new SRInt(Type::IORES, true);

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
