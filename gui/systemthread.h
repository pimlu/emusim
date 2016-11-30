#ifndef SYSTEMTHREAD_H
#define SYSTEMTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../sim/system.h"

namespace gui {

class SystemThread {
    std::thread *t;
    std::mutex pausemtx;
    std::condition_variable cv;
    std::mutex schedmtx;
    void tRun();
    bool paused = true;
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
};

}

#endif // SYSTEMTHREAD_H
