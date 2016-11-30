#include "systemthread.h"

#include <chrono>

namespace gui {

using std::unique_lock;
using ulock_recmtx = std::unique_lock<std::recursive_mutex>;

SystemThread::SystemThread(sim::System *system, int hz) : system(system), hz(hz) {
    schedmtx.lock();
    t = new std::thread(&SystemThread::tRun, this);
}
SystemThread::~SystemThread() {
    t->detach();
    delete t;
}
void SystemThread::tRun() {
    //unique_lock<mutex> lck(pausemtx);
    using namespace std::chrono;
    while(true) {
        schedmtx.lock();
        steady_clock::time_point time = steady_clock::now();
        system->sched->doSim(hz/PERIOD, paused); //does cycles, then sleeps till next period
        schedmtx.unlock();
        std::this_thread::sleep_until(time + std::chrono::milliseconds(1000/PERIOD));
    }
}
void SystemThread::run() {
    paused = false;
    schedmtx.unlock();
}
void SystemThread::pause() {
    paused = true;
    schedmtx.lock();
}
bool SystemThread::toggle() {
    if(isPaused()) run();
    else pause();
    return paused;
}

bool SystemThread::isPaused() {
    return paused;
}
int SystemThread::add(sim::Process *p, std::string name) {
    ulock_recmtx lck(schedmtx);
    return system->sched->add(p, name);
}
void SystemThread::remove(int pid) {
    ulock_recmtx lck(schedmtx);
    return system->sched->remove(pid);
}
int SystemThread::exec(std::string name) {
    ulock_recmtx lck(schedmtx);
    return system->exec(name);
}
unsigned long long SystemThread::step() {
    ulock_recmtx lck(schedmtx);
    bool p = false;
    return system->sched->doSim(1, p);
}


}
