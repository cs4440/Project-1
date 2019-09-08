/*
 * Shell program that accepts commands and arguments to call processes
 */
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait()
#include <unistd.h>     // exec()
#include <unistd.h>     // fork()
#include <algorithm>    // copy()
#include <cstring>      // strncmp()
#include <iostream>     // cin
#include <sstream>      // stringstream
#include <string>       // getline()
#include <vector>       // vector

// delete entire char** and its child elements
void deallocate_all(char** arr, std::size_t size) {
    for(std::size_t i = 0; i < size; ++i) delete[] arr[i];
    delete[] arr;
}

int main() {
    char* cstr;               // hold cstring version of token for args
    char** args;              // hold array of cstr for exec
    std::string line, token;  // user line input and token from line
    std::stringstream ss;     // for line tokenization
    std::vector<char*> vec;   // vector of cstr

    while(std::getline(std::cin, line)) {
        // get tokens from line and push to vector
        ss << line;  // set line to sstream
        while(ss >> token) {
            cstr = new char[token.size() + 1];
            std::copy(token.begin(), token.end(), cstr);
            cstr[token.size()] = '\0';  // nul termiante!
            vec.push_back(cstr);
        }
        ss.clear();  // reset sstream flags

        // assign vector elements to arguments array
        args = new char*[vec.size() + 1];
        for(std::size_t i = 0; i < vec.size(); ++i) args[i] = vec[i];
        args[vec.size()] = NULL;  // last argument needs to be NULL

        // check for quit() command to exit shell and deallocate all
        if(std::strncmp(args[0], "quit()", 6) == 0 ||
           std::strncmp(args[0], "exit()", 6) == 0) {
            deallocate_all(args, vec.size());

            break;
        }

        pid_t pid = fork();

        if(pid < 0) {
            std::cout << "Fork failed." << std::endl;
            deallocate_all(args, vec.size());

            return 1;
        } else if(pid == 0) {
            execvp(args[0], args);  // call exec with arguments

            // if exec fails here, then kill child with return 1
            std::cout << "Exec error." << std::endl;
            deallocate_all(args, vec.size());

            return 1;
        } else {
            wait(&pid);
        }

        deallocate_all(args, vec.size());
        vec.clear();  // reset vector
    }

    return 0;
}
