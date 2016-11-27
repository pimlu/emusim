#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>
namespace sim {

using std::string;

class FileSystem {
public:
    //creates a new FileSystem object.  directories are not supported.  path
    //points to the real-world directory containing our list of files
    FileSystem(string path);
    string path;
    //reads len bytes at index seek of the file.  so it reads [seek, seek+len)
    char* readFile(string name, int seek, int len);
    //writes to [seek, seek+len) with the characters from *data
    //if the file does not exist, creates it.  returns whether file was created
    bool writeFile(string name, int seek, int len, char *data);
    //deletes a file.  if the file does not exist, does nothing.
    //returns whether file was deleted
    bool deleteFile(string name);
    //lists files inside our path directory - stores them inside a vector
    std::vector<string> listFiles();
};

}
#endif // FILESYSTEM_H
