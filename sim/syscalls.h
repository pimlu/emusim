#ifndef SYSCALLS_H
#define SYSCALLS_H

//data protocol between processes and System.

namespace sim {

enum Type { NONE, END, READ};

struct Syscall {
    Syscall(Type type) : type(type) {}
    Type type;
};
struct SCEnd : Syscall {
    SCEnd(int retCode) : Syscall(Type::END), retCode(retCode) {}
    int retCode;
};
struct SCRead : Syscall {
    SCRead(char *file, int seek, int len) :
        Syscall(Type::READ), file(file), seek(seek), len(len) {}
    char *file;
    int seek, len;
};

struct Sysres {
    Type type;
    Sysres(Type type) : type(type) {}
};

struct SRRead : Sysres {
    SRRead(char* text, int len) :
        Sysres(Type::READ), text(text), len(len) {}
    char *text;
    int len;
};

}

#endif // SYSCALLS_H
