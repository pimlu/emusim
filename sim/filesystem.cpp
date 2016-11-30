#include "filesystem.h"
#include <fstream>
#include <iostream>



namespace sim {
    FileSystem::FileSystem(string path) : path(path){

    }

    char* FileSystem:: readFile(string name, int seek, int len){
        std::ifstream infile(((path + "/" + name).c_str()), std::ifstream::binary);
        infile.seekg(seek);

        char* buffer= new char[len];
        infile.read(buffer, len);
        return buffer;

    }

    bool FileSystem::writeFile(string name, int seek, int len, char *data){
      std::ofstream outfile((path + "/" + name).c_str(), std::ofstream::binary | std::ios::in | std::ios::out);
      outfile.seekp(seek);
      outfile.write(data,len);
      return !outfile.bad();

    }

    bool FileSystem::deleteFile(string name){
       return(remove((path + "/" + name).c_str())==0);
       
   }
}
