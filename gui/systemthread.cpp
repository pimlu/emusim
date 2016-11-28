#include "systemthread.h"

#include <chrono>

namespace gui {

SystemThread::SystemThread(sim::System *system, int hz) : system(system), hz(hz) {
    t = new std::thread(&SystemThread::tRun, this);
}
SystemThread::~SystemThread() {
    t->detach();
    delete t;
}
void SystemThread::tRun() {
    std::unique_lock<std::mutex> lck(mtx);
    using namespace std::chrono;
    while(true) {
        while(paused) cv.wait(lck); //locks until ran
        steady_clock::time_point time = steady_clock::now();
        system->sched->doSim(hz/PERIOD, paused); //does cycles, then sleeps till next period
        std::this_thread::sleep_until(time + std::chrono::milliseconds(1000/PERIOD));
    }
}
void SystemThread::run() {
    paused = false;
    cv.notify_all();
}
void SystemThread::pause() {
    paused = true;
}
bool SystemThread::toggle() {
    if(isPaused()) run();
    else pause();
    return paused;
}

bool SystemThread::isPaused() {
    return paused;
}


}
