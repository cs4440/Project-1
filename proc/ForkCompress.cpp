/*
 * Demonstration of fork() process with MyCompress program
 *
 * DEAILS
 * ------
 * Forks a process and passes arguments to exec call of MyCompress
 */
#include <limits.h>     // PATH_MAX
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait()
#include <unistd.h>     // fork(), execl()
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    char default_src[] = "res/sample.txt",
         default_dest[] = "res/forkcompress.txt";
    char *src = default_src, *dest = default_dest;

    // check for argument overrides
    if(argc > 1) src = argv[1];
    if(argc > 2) dest = argv[2];

    pid_t pid = fork();  // fork process and return int state

    if(pid < 0) {
        std::cerr << "fork() failed" << std::endl;
        return 1;
    } else if(pid == 0) {
        std::cout << "I am child (pid: " << getpid() << ")" << std::endl;
        std::cout << "Calling MyCompress program" << std::endl;

        // get current path
        char cwd[PATH_MAX];
        if(getcwd(cwd, PATH_MAX) == NULL)
            std::cerr << "getcwd() failed" << std::endl;

        // create absolute path for program
        std::string full_path(cwd);
        full_path += "/MyCompress";

        // call exec to spawn MyCompress
        execl(full_path.c_str(), full_path.c_str(), src, dest, NULL);

        std::cerr << "exec() failed" << std::endl;
        return 1;
    } else {
        int pid_wait = wait(NULL);  // waiting for child to complete
        std::cout << "I am parent (pid: " << getpid()
                  << "), waiting for child (pid: " << pid_wait << ")"
                  << std::endl;
    }

    return 0;
}
