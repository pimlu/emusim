#ifndef SYSCALLS_H
#define SYSCALLS_H

//data protocol between processes and System.

struct Syscall {
    enum Type { NONE, END, READ };
    Syscall(Type type) : type(type) {}
    Type type;
};
struct SCEnd : Syscall {
    SCEnd(int retCode) : Syscall(Type::END), retCode(retCode) {}
    int retCode;
};
struct SCRead : Syscall {
    SCRead(char *file, int len, int seek) :
        Syscall(Type::READ), file(file), len(len), seek(seek) {}
    char *file;
    int len, seek;
};

struct Sysres {
    enum Type { NONE, READ};
    Type type;
    Sysres(Type type) : type(type) {}
};

struct SRRead : Sysres {
    SRRead(char* text, int len) :
        Sysres(Type::READ), text(text), len(len) {}
    char *text;
    int len;
};

#endif // SYSCALLS_H
