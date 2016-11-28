#ifndef SYSCALLS_H
#define SYSCALLS_H

//data protocol between processes and System.

namespace sim {

enum Type {
    NONE, END, PRINT, INPUT,
    PRINTN, INPUTN, READ, WRITE,
    IORES
};

struct Syscall {
    Syscall(Type type) : type(type) {}
    Type type;
};
struct SCInt : Syscall {
    SCInt(Type t, int val) : Syscall(t), val(val) {}
    int val;
};

struct SCString : Syscall {
    SCString(Type t, char *str, int len) : Syscall(t), str(str), len(len) {}
    char *str;
    int len;
};

struct SCRead : Syscall {
    SCRead(char *file, int seek, int len) :
        Syscall(Type::READ), file(file), seek(seek), len(len) {}
    char *file;
    int seek, len;
};

struct SCWrite : Syscall {
    SCWrite(char *file, int seek, char *data, int len) :
        Syscall(Type::WRITE), file(file), seek(seek), data(data), len(len) {}
    char *file;
    int seek;
    char *data;
    int len;
};

struct Sysres {
    Type type;
    Sysres(Type type) : type(type) {}
};

struct SRInt : Sysres {
    SRInt(Type t, int val) : Sysres(t), val(val) {}
    int val;
};

struct SRString : Sysres {
    SRString(Type t, char* text, int len) :
        Sysres(t), text(text), len(len) {}
    char *text;
    int len;
};

}

#endif // SYSCALLS_H
