/*
 * Simple shell program that can only accept commands to call programs
 */
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait()
#include <unistd.h>     // fork()
#include <cstring>      // strncmp()
#include <iostream>     // cin()

int main() {
    std::string token;

    while(token != "exit") {
        // user input
        std::cout << "# ";
        std::cin >> token;

        // allocation
        char **args = new char *[2];
        args[0] = new char[token.size() + 1];

        // copy token to cstring arg[0]
        std::copy(token.begin(), token.end(), args[0]);
        args[0][token.size()] = '\0';  // nul terminate!
        args[1] = NULL;

        // check for quit() command to exit shell and deallocate all
        if(token == "exit") {
            // deallocate all
            delete[] args[0];
            delete[] args;

            break;
        }

        pid_t pid = fork();

        if(pid < 0) {
            std::cout << "Fork failed." << std::endl;

            // deallocate all
            delete[] args[0];
            delete[] args;
            return 1;
        } else if(pid == 0) {
            execvp(args[0], args);

            // if exec fails here, then kill child with return 1
            std::cout << "Exec error." << std::endl;

            // deallocate all
            delete[] args[0];
            delete[] args;

            return 1;
        } else {
            wait(&pid);
        }

        // deallocate all
        delete[] args[0];
        delete[] args;
    }
}
