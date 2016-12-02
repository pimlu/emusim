#ifndef SYSTEMTHREAD_H
#define SYSTEMTHREAD_H

#include <thread>
#include <mutex>
#include <utility>
#include <vector>

#include "../sim/system.h"

namespace gui {

using ulock_recmtx = std::unique_lock<std::recursive_mutex>;

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
    void tRun();
    bool paused = true;
    bool tLoop = true;
public:
    std::recursive_mutex schedmtx;
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
    int exec(std::string name, int priority = 0);
    int step(int n = 1);
    void reset();
    std::vector<ProcData> getProcs();
    ulock_recmtx getLock();
};

}

#endif // SYSTEMTHREAD_H
