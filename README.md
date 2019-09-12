# CS 4440 - Operating Systems

Project for Operating Systems class using C/C++ programming under linux

## Project members

- Adrian Palomares
- Daniel Gomez
- Thuan Tang

## Project 1

Realizing concurrency using Unix processes and threads

- MyCompress - Simple binary compression program
- MyDecompress - Simple binary decompression program
- ForkCompress - Fork child process to call MyCompress
- PipeCompress - Pipe program where parent process passes source file for child process to compress file
- ParFork - Concurrency program: split compression by n calls of MyCompress via fork()
- MinShell - Minimal shell that accepts commands to call processes
- MoreShell - More featured shell that accepts commands and arguments to call processes
- DupShell - shell that uses dup2() to connect pipes between commands for operator |
