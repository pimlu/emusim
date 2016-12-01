#ifndef SYSTEMTHREAD_H
#define SYSTEMTHREAD_H

#include <thread>
#include <mutex>
#include <utility>
#include <vector>

#include "../sim/system.h"

namespace gui {

enum ProcStatus { JOB, RUNNING, WAITING, BLOCKED, ENDED };
struct ProcData {
    ProcData(sim::Scheduler *s, sim::Process *p, ProcStatus status) :
        pcb(s->pcbs[p]), status(status), memory(p->memory)  {}
    sim::PCB pcb;
    ProcStatus status;
    int memory;
};

class SystemThread {
    std::thread *t;
    std::recursive_mutex schedmtx;
    void tRun();
    bool paused = true;
    bool tLoop = true;
public:
    const int PERIOD = 10;
    SystemThread(sim::System *system, int hz);
    ~SystemThread();
    sim::System *system;
    int hz;
    void run();
    void pause();
    bool isPaused();
    bool toggle();
    int add(sim::Process *p, std::string name);
    int add(std::string name);
    void remove(sim::Process *p);
    void remove(int pid);
    sim::Process* find(int pid);
    int exec(std::string name);
    int step(int n = 1);
    std::vector<ProcData> getProcs();
};

}

#endif // SYSTEMTHREAD_H
