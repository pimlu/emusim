#include "system.h"
#include "../emu/emuprocess.h"
namespace sim {

using std::string;

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
        Process *p = req.first;
        Syscall *sc = req.second;
        ProcRes res(req.first, nullptr);
        PCB &pcb = sched->pcbs[p];
#define DOIO do { if(cycles<IOCOST) return ret; \
        cycles -= IOCOST; \
        pcb.ioreqs++; } while(0);
        //we use blocks inside cases to avoid jumping over initialization
        switch(sc->type) {
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
                DOIO;
                pcb.obytes += scs->len;

                out.write(scs->str, scs->len);
                res.second = new SRInt(Type::IORES, true);

                }break;
            case Type::INPUT:
                {DOIO;

                char *buffer = new char[256];
                in.getline(buffer, 256);
                int len = in.gcount();
                pcb.ibytes += len;
                res.second = new SRString(Type::IORES, buffer, len);

                }break;
            case Type::PRINTN:
                {SCInt *sci = (SCInt*) sc;
                DOIO;
                pcb.obytes += 2;

                out << sci->val;

                res.second = new SRInt(Type::IORES, 1);

                }break;
            case Type::INPUTN:
                {DOIO;
                pcb.obytes += 2;

                int n;
                in >> n;

                res.second = new SRInt(Type::IORES, n);

                }break;
            case Type::READ:
                {SCRead *scr = (SCRead*) sc;
                DOIO;
                pcb.ibytes += scr->len;
                char *data = fs.readFile(string(scr->file, scr->flen), scr->seek, scr->len);

                res.second = new SRString(Type::IORES, data, scr->len);
                }break;
            case Type::WRITE:
                {SCWrite *scw = (SCWrite*) sc;
                DOIO;
                pcb.obytes += scw->len;
                bool good = fs.writeFile(string(scw->file, scw->flen), scw->seek, scw->len, scw->data);

                res.second = new SRInt(Type::IORES, good);
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
