#include "system.h"
#include "../emu/emuprocess.h"
namespace sim {
using std::priority_queue;

using std::string;


ProcCall blockq::top() {
    return data[0];
}

void blockq::pop(Scheduler *sched) {
    int best = 9999;
    size_t besti = 0;

    for(size_t i=0; i<data.size(); i++) {
        int cur = sched->pcbs[data[i].first].priority;
        if(cur<best) {
            best = cur;
            besti = i;
        }
    }
    data.erase(data.begin()+besti);

}

void blockq::push(ProcCall pc) {
    data.push_back(pc);
}

bool blockq::remove(Process *p) {
    bool del = false;
    for(size_t i=0; i<data.size(); i++) {
        if(data[i].first == p) {
            data.erase(data.begin()+i);
            del = true;
        }
    }
    return del;
}
bool blockq::empty() {
    return data.size() == 0;
}

void blockq::clear() {
    data.clear();
}
std::vector<ProcCall>& blockq::getVec() {
    return data;
}


std::function<void(const char *s)> noop = [](const char *s) -> void { };

System::System(int memory, int quantum, std::istream &in, std::ostream &out, std::string path,
               std::function<void(const char *s)> &log = noop) :
               log(log), fs(path), memory(memory),in(in), out(out),
               blockQueue(/*[&](ProcCall a, ProcCall b) -> bool {
                    return sched->pcbs[a.first].priority < sched->pcbs[b.first].priority;
               }*/) {
    sched = new Scheduler(this, quantum);
}

System::~System() {
    delete sched;
}

void System::addBlock(ProcCall pc) {
    blockQueue.push(pc);
}
/*std::vector<ProcCall> System::getBlock() {
    return Container(blockQueue);
}*/

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

        ProcCall req = blockQueue.top();
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
        blockQueue.pop(this->sched);
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

void System::reset() {
    blockQueue.clear();
    finishQueue.clear();
    usedMem = 0;
    sched->jobQueue.clear();
    sched->waitQueue.clear();
    sched->curProc = {nullptr, nullptr};
    sched->cycle = sched->cyclesLeft = 0;
    sched->curpid = 1;
    sched->pcbs.clear();
}


}
