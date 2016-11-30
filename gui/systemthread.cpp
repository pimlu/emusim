#include "systemthread.h"

#include <chrono>

namespace gui {

using std::unique_lock;
using std::mutex;

SystemThread::SystemThread(sim::System *system, int hz) : system(system), hz(hz) {
    t = new std::thread(&SystemThread::tRun, this);
}
SystemThread::~SystemThread() {
    t->detach();
    delete t;
}
void SystemThread::tRun() {
    unique_lock<mutex> lck(pausemtx);
    using namespace std::chrono;
    while(true) {
        while(paused) cv.wait(lck); //locks until ran

        steady_clock::time_point time = steady_clock::now();
        schedmtx.lock();
        system->sched->doSim(hz/PERIOD, paused); //does cycles, then sleeps till next period
        schedmtx.unlock();
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
int SystemThread::add(sim::Process *p, std::string name) {
    unique_lock<mutex> lck(schedmtx);
    return system->sched->add(p, name);
}
void SystemThread::remove(int pid) {
    unique_lock<mutex> lck(schedmtx);
    return system->sched->remove(pid);
}
int SystemThread::exec(std::string name) {
    unique_lock<mutex> lck(schedmtx);
    return system->exec(name);
}



}
