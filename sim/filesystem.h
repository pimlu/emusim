#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>
namespace sim {


class FileSystem {
public:
    //creates a new FileSystem object.  directories are not supported.  path
    //points to the real-world directory containing our list of files
    FileSystem(std::string path);
    std::string path;
    //reads len bytes at index seek of the file.  so it reads [seek, seek+len)
    //it will be the caller's responsibility to delete the returned pointer, not FileSystem
    char* readFile(std::string name, int seek, int len);
    //writes to [seek, seek+len) with the characters from *data
    //if the file does not exist, creates it.  returns whether file was created
    bool writeFile(std::string name, int seek, int len, char *data);
    //deletes a file.  if the file does not exist, does nothing.
    //returns whether file was deleted
    bool deleteFile(std::string name);
    //returns the length of the file.  uses seekg but works on unix systems.
    int fileLen(std::string name);
    //lists files inside our path directory - stores them inside a vector
    std::vector<std::string> listFiles();
};

}
#endif // FILESYSTEM_H
