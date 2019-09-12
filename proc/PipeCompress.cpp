/*
 * Demonstration of pipe() between forked processes of parent and child
 *
 * DETAILS
 * ------
 * Forks a process where parent reads a source file. Using pipe(), parent will
 * send the source file to child to perform compression.
 */
#include <stdio.h>                   // fopen()
#include <sys/types.h>               // pid_t
#include <unistd.h>                  // fork(), pipe()
#include <iostream>                  // stream
#include "../include/compression.h"  // compress()

int main(int argc, char *argv[]) {
    char default_src[] = "res/sample.txt",
         default_dest[] = "res/pipecompress.txt";
    char *src = default_src, *dest = default_dest;
    int chr,    // character from file
        fd[2];  // pipe file descriptors
    pid_t pid;

    // check for argument overrides
    if(argc > 1) src = argv[1];
    if(argc > 2) dest = argv[2];

    if(pipe(fd) != 0) {
        std::cerr << "pipe() failed" << std::endl;
        return 1;
    }

    pid = fork();

    if(pid < 0) {
        std::cerr << "fork() failed" << std::endl;
        return 1;
    } else if(pid == 0) {
        close(fd[1]);                          // close pipe write
        FILE *pipe_read = fdopen(fd[0], "r"),  // open pipe read
            *fdest = fopen(dest, "wb");        // open destination file

        compress(pipe_read, fdest);

        fclose(pipe_read);
        fclose(fdest);
    } else {
        close(fd[0]);                           // close pipe read
        FILE *pipe_write = fdopen(fd[1], "w"),  // open pipe write
            *fsrc = fopen(src, "rb");           // open source file

        // read from source file and write to pipe
        while((chr = fgetc(fsrc)) != EOF) fputc(chr, pipe_write);

        fclose(pipe_write);
        fclose(fsrc);
    }

    return 0;
}
