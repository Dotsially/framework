#ifndef FILEIO
#define FILEIO
#include "common.cpp"

//Goes through all files in a directory
std::vector<std::string> ReadDirectory(std::string path){
    std::vector<std::string> files;

    DIR *directory;
    struct dirent *entry;
    if ((directory = opendir (path.c_str())) != NULL) {
        while ((entry = readdir(directory)) != NULL) {
            char* fileName = entry->d_name;
            int nameLength = entry->d_namlen;
            std::string file(fileName, nameLength);
            
            if (file != "." && file != "..") {
                std::cout << file << std::endl;
                files.push_back(file);        
            }
        }
        closedir (directory);
    } else {
        std::cout << "Could not open directory" << std::endl;
    }

    return files;
}


#endif