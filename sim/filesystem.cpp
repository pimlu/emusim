#include "filesystem.h"
#include <fstream>
#include <iostream>



namespace sim {
    using std::string;
    FileSystem::FileSystem(string path) : path(path) {

    }

    char* FileSystem:: readFile(string name, int seek, int len) {
        std::ifstream infile(path + "/" + name, std::ifstream::binary);
        infile.seekg(seek);
        char* buffer= new char[len];
        infile.read(buffer, len);
        if(infile.bad()) {
            delete[] buffer;
            return nullptr;
        }
        return buffer;
    }

    bool FileSystem::writeFile(string name, int seek, int len, const char *data) {
        string pname = path + "/" + name;
        if(!std::ifstream(pname).good()) { std::ofstream touch(pname); }

        std::ofstream outfile(pname, std::ofstream::binary | std::ios::in | std::ios::out);
        outfile.seekp(seek);
        outfile.write(data,len);
        return !outfile.bad();
    }

    bool FileSystem::deleteFile(string name) {
        return(remove((path + "/" + name).c_str())==0);
    }

    int FileSystem::fileLen(string name) {
        std::ifstream in(path + "/" + name,
                         std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }
}
