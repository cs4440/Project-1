#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait()
#include <unistd.h>     // exec()
#include <unistd.h>     // fork()
#include <iostream>     // cin
#include <string>       // getline()
#include <vector>       // vector
#include "../include/parser.h"

class Shell {
public:
    Shell() {}

    void run();

private:
    Parser _parser;
};

#endif  // SHELL_H
