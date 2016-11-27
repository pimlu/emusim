#ifndef SYSTEMTHREAD_H
#define SYSTEMTHREAD_H

#include <thread>
#include <mutex>

#include "../sim/system.h"

namespace gui {

class SystemThread {
    std::thread *t;
    std::mutex mtx;
    std::condition_variable cv;
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
};

}

#endif // SYSTEMTHREAD_H
