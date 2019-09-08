#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
    int fd[2], rt, pid;

    if(argc < 3) {
        std::cout << "Insufficient arguments" << std::endl;
    } else {
        rt = pipe(fd);
        pid = fork();

        std::cout << "fd[0]: " << fd[0] << ", fd[1]: " << fd[1] << std::endl;

        if(rt < 0 || pid < 0)
            std::cerr << "pipe() or fork() failed." << std::endl;
        else if(pid == 0) {
            int buf_size = 5;
            char *buf = new char[buf_size];

            std::cout << "Reading from file" << std::endl;
            close(fd[0]);  // close file descriptor read

            // open first argument as file input
            std::fstream fin(argv[1], std::ios::in | std::ios::binary);

            // read input file and write to fd[1]
            fin.read(buf, buf_size - 1);
            buf[fin.gcount()] = '\0';
            write(fd[1], buf, fin.gcount());

            while(fin.gcount()) {
                fin.read(buf, buf_size - 1);
                buf[fin.gcount()] = '\0';
                write(fd[1], buf, fin.gcount());
            }

            delete[] buf;
        } else {
            int pid_wait = wait(NULL);
            std::cout << "Writing to file" << std::endl;
            close(fd[1]);  // close file ddscriptor write

            std::fstream fout(argv[2], std::ios::in | std::ios::out |
                                           std::ios::binary | std::ios::trunc);

            // get current path
            char cwd[PATH_MAX];
            if(getcwd(cwd, PATH_MAX) == NULL)
                std::cerr << "getcwd() failed." << std::endl;

            // acreate absolute path for program
            std::string full_path(cwd);
            full_path += "/read";

            // call exec to spawn MyCompress
            int ret = execl(full_path.c_str(), full_path.c_str(), &fd[0],
                            argv[2], NULL);

            if(ret < 0) std::cout << "exec error" << std::endl;
        }
    }

    return 0;
}
