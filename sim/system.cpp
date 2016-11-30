#include "system.h"
#include "../emu/emuprocess.h"
namespace sim {

System::System(int memory, int quantum, std::istream &in, std::ostream &out, std::string path) :
    fs(path), memory(memory), in(in), out(out) {
    sched = new Scheduler(this, quantum);
}

System::~System() {
    delete sched;
}

const int IOCOST = 30;
bool System::runSyscalls(int c) {
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
                res.second = new SRInt(Type::END, sci->val);

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

                out << sci->val;

                res.second = new SRInt(Type::IORES, 1);

                }break;
            case Type::INPUTN:
                {if(cycles<IOCOST) return ret;
                cycles -= IOCOST;

                int n;
                in >> n;

                res.second = new SRInt(Type::IORES, n);

                }break;
        }
        ret = true;
        blockQueue.pop_front();
        delete sc;
        if(res.first) finishQueue.push_back(res);
    }

    return ret;
}

int System::exec(std::string name) {
    int len = fs.fileLen(name);
    if(!~len) return -1;
    char *data = fs.readFile(name, 0, len);
    if(!data) return -1;
    emu::EmuProcess *p = new emu::EmuProcess(data, len);
    return sched->add(p, name);
}


}
