/*
 * Demonstration of fork() process with MyCompress program
 */
#include <limits.h>    // PATH_MAX
#include <sys/wait.h>  // wait()
#include <unistd.h>    // fork(), execl()
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    if(argc < 3) {
        std::cerr << "Insufficient arguments." << std::endl;
        return 1;
    }

    int pid = fork();  // fork process and return int state

    if(pid < 0) {
        std::cerr << "fork() process failed." << std::endl;
        return 1;
    }

    if(pid == 0) {
        std::cout << "I am child (pid: " << getpid() << ")." << std::endl;
        std::cout << "Calling for compress program." << std::endl;

        // get current path
        char cwd[PATH_MAX];
        if(getcwd(cwd, PATH_MAX) == NULL)
            std::cerr << "getcwd() failed." << std::endl;

        // create absolute path for program
        std::string full_path(cwd);
        full_path += "/MyCompress";

        // call exec to spawn MyCompress
        int rt =
            execl(full_path.c_str(), full_path.c_str(), argv[1], argv[2], NULL);

        if(rt < 0) std::cerr << "execl() failed" << std::endl;
    } else {
        int pid_wait = wait(NULL);  // waiting for child to complete
        std::cout << "I am parent (pid: " << getpid()
                  << "), waiting for child (pid: " << pid_wait << ")"
                  << std::endl;
    }

    return 0;
}
