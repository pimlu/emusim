#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <deque>
#include <functional>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

namespace sim { class System; }
#include "scheduler.h"
#include "process.h"
#include "filesystem.h"

namespace sim {

using std::deque;

//https://stackoverflow.com/questions/19467485/c-priority-queue-removing-element-not-at-top
template<typename T>
class vecpq : public std::priority_queue<T, std::vector<T>, const std::function <bool (ProcCall, ProcCall)>&> {
public:
    vecpq(const std::function <bool (ProcCall, ProcCall)> &cmp);
    bool remove(const T& value);
    template<typename P>
    bool remove_if(P pred);
    std::vector<T>& getVec();
};


template<typename T>
vecpq<T>::vecpq(const std::function <bool (ProcCall, ProcCall)> &cmp) :
    std::priority_queue<T, std::vector<T>, const std::function <bool (ProcCall, ProcCall)>&>(cmp) {}

template<typename T>
bool vecpq<T>::remove(const T& value) {
    auto it = std::find(this->c.begin(), this->c.end(), value);
    if (it != this->c.end()) {
        this->c.erase(it);
        std::make_heap(this->c.begin(), this->c.end(), this->comp);
        return true;
    } else {
        return false;
    }
}

template<typename T>
template<typename P>
bool vecpq<T>::remove_if(P pred) {
    auto it = std::find_if(this->c.begin(), this->c.end(), pred);
    if (it != this->c.end()) {
        this->c.erase(it);
        std::make_heap(this->c.begin(), this->c.end(), this->comp);
        return true;
    } else {
        return false;
    }
}
template<typename T>
std::vector<T>& vecpq<T>::getVec() {
    return this->c;
}

using blockq = vecpq<ProcCall>;

class System {
public:
    System(int memory, int quantum, std::istream &in, std::ostream &out,
           std::string path, std::function<void(const char *s)> &log);
    ~System();
    std::function<void(const char *s)> &log;
    void addBlock(ProcCall pc);
    bool runSyscalls(int c); //passes time for the system, returns true if an entry finished
    int exec(std::string name);
    void reset();
    std::vector<ProcCall> getBlock();
    FileSystem fs;
    int memory, usedMem=0;
    std::istream &in;
    std::ostream &out;
    blockq blockQueue; //blocked - syscall result (usually IO)
    std::deque<ProcRes> finishQueue; //processes ready to be put in the waiting queue
    int cycles = 0; //how far along in the blockQueue we are
    Scheduler *sched;
};


}

#endif // SYSTEM_H
