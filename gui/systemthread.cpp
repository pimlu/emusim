#include "systemthread.h"

#include <chrono>

namespace gui {

SystemThread::SystemThread(sim::System *system, int hz) : system(system), hz(hz) {
    schedmtx.lock();
    t = new std::thread(&SystemThread::tRun, this);
}
SystemThread::~SystemThread() {
    tLoop = false;
    if(paused) {
        schedmtx.unlock();
    }
    t->join();
    delete t;
}
void SystemThread::tRun() {
    //unique_lock<mutex> lck(pausemtx);
    using namespace std::chrono;
    while(tLoop) {
        schedmtx.lock();
        steady_clock::time_point time = steady_clock::now();
        system->sched->doSim(hz/PERIOD, paused); //does cycles, then sleeps till next period
        schedmtx.unlock();
        if(tLoop) std::this_thread::sleep_until(time + std::chrono::milliseconds(1000/PERIOD));
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
int SystemThread::step(int n) {
    ulock_recmtx lck(schedmtx);
    bool p = false;
    return system->sched->doSim(n, p);
}
void SystemThread::reset() {
    ulock_recmtx lck(schedmtx);
    system->reset();
}

std::vector<ProcData> SystemThread::getProcs() {
    ulock_recmtx lck(schedmtx);
    std::vector<ProcData> vec;
    sim::Scheduler *s = system->sched;
    if(s->curProc.first) {
        vec.push_back(ProcData(s, s->curProc.first, ProcStatus::RUNNING));
    }
    for(sim::Process *p : s->jobQueue) {
        vec.push_back(ProcData(s, p, ProcStatus::JOB));
    }
    for(sim::ProcRes pr : s->waitQueue) {
        vec.push_back(ProcData(s, pr.first, ProcStatus::WAITING));
    }
    for(sim::ProcRes pr : system->finishQueue) {
        vec.push_back(ProcData(s, pr.first, pr.second->type == sim::Type::END ?
                                   ProcStatus::ENDED : ProcStatus::WAITING));
    }
    for(sim::ProcCall ps : system->blockQueue) {
        vec.push_back(ProcData(s, ps.first, ProcStatus::BLOCKED));
    }
    return vec;
}

ulock_recmtx SystemThread::getLock() {
    return ulock_recmtx(schedmtx);
}


}
