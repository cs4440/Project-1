/*
 * Concurrency: calls MyCompress n times to work on large file via fork()
 *
 * DETAILS
 * -------
 * Splits file by n times. Calls MyCompress to work on each split file
 * via fork(). Then merge all the compression results to final output.
 */
#include <sys/stat.h>                // stat(), mkdir()
#include <sys/types.h>               // pid_t
#include <sys/wait.h>                // wait()
#include <unistd.h>                  // fork()
#include <cstdio>                    // fopen()
#include <cstdlib>                   // atoi()
#include <cstring>                   // strlen()
#include <iostream>                  // stream
#include <vector>                    // vector
#include "../include/compression.h"  // compress()
#include "../include/timer.h"        // Timer class

// merges files without fixing inconsistence truncations
// @param files - a vector of char strings
// @param dest - destination file
void merge(std::vector<char*>& files, FILE* dest);

int main(int argc, char* argv[]) {
    bool report_timer = true;
    char default_src[] = "res/sample.txt",
         default_dest[] = "res/par_fork_compress.txt";
    char *src = default_src, *dest = default_dest;
    char* src_arr = nullptr;               // array to hold source content
    unsigned n = 6;                        // default n split
    struct stat buffer;                    // directory status info
    FILE *fsrc = nullptr,                  // source FILE*
        *fdest = nullptr;                  // dest FILE*
    long int file_size = 0,                // total file size
        split_size = 0,                    // split file size
        read_index = 0;                    // source array index
    std::vector<char*> splits;             // resultant array for compression
    int *fd = nullptr, *cur_fd = nullptr;  // file descriptors for pipe()
    pid_t pid = -1;                        // process id for fork()
    timer::ChronoTimer ct;                 // chrono timer

    std::cout << "priting args:\n";
    for(int i = 0; i < argc; ++i) std::cout << argv[i] << std::endl;

    // check for argument overrides
    if(argc > 1) src = argv[1];
    if(argc > 2) dest = argv[2];
    if(argc > 3) n = atoi(argv[3]);

    // check if directory tmp/ exists, stat() returns 0 if exists
    if(stat("tmp/", &buffer) != 0) mkdir("tmp/", 0700);

    // open source file
    fsrc = fopen(src, "rb");

    if(!fsrc) {
        std::cerr << "Cannot find input file" << std::endl;
        return 1;
    }

    // find total file size
    fseek(fsrc, 0, SEEK_END);
    file_size = ftell(fsrc);
    split_size = read_index = file_size / n;
    fseek(fsrc, 0, SEEK_SET);

    if(!file_size) {
        std::cerr << "Input file is empty" << std::endl;
        fclose(fsrc);
        return 1;
    } else if(file_size < n) {
        std::cerr << "Input file is smaller than n concurrency" << std::endl;
        fclose(fsrc);
        return 1;
    }

    std::cout << "Starting fork() concurrency compression of " << n
              << std::endl;

    // start timer
    ct.start();

    // copy file to array
    src_arr = new char[file_size + 1];
    read_to_arr(fsrc, src_arr);

    // create file descriptors for n forks
    fd = new int[2 * n];

    for(unsigned i = 0; i < n; ++i) {
        // on last file, recalc split size from integer division loss
        if(i == n - 1) split_size = (file_size - (split_size * i));

        splits.emplace_back(new char[split_size + 1]);

        // get current file descriptor
        pipe(fd + 2 * i);
        cur_fd = fd + (2 * i);

        // fork a process to compress split files
        pid = fork();

        if(pid < 0) {
            std::cerr << "fork() failed" << std::endl;

            // free allocation
            for(std::size_t i = 0; i < splits.size(); ++i) delete[] splits[i];
            delete[] fd;
            delete[] src_arr;
            fclose(fsrc);

            return 1;
        } else if(pid == 0) {
            memset(splits[i], 0, split_size);
            compress_arr(src_arr + (read_index * i), splits[i], split_size);

            close(cur_fd[0]);
            FILE* write_pipe = fdopen(cur_fd[1], "wb");
            fwrite(splits[i], sizeof(char), split_size, write_pipe);
            fclose(write_pipe);

            // free allocation
            for(std::size_t i = 0; i < splits.size(); ++i) delete[] splits[i];
            delete[] fd;
            delete[] src_arr;
            fclose(fsrc);

            return 0;
        } else {
            close(cur_fd[1]);
            FILE* read_pipe = fdopen(cur_fd[0], "rb");

            int chr = 0, index = 0;
            while((chr = fgetc(read_pipe)) != EOF) splits[i][index++] = chr;
            splits[i][index] = '\0';

            fclose(read_pipe);
        }
    }
    for(unsigned i = 0; i < n; ++i) wait(nullptr);

    // open output file and merge all compressed_files to output
    fdest = fopen(dest, "w+b");
    merge(splits, fdest);

    // stop timer
    ct.stop();

    delete[] fd;
    for(std::size_t i = 0; i < splits.size(); ++i) delete[] splits[i];
    delete[] src_arr;
    fclose(fdest);
    fclose(fsrc);

    std::cout << "Concurrency compression complete" << std::endl;

    if(report_timer)
        std::cout << "Compression time: " << ct.seconds() << " seconds"
                  << std::endl;

    return 0;
}

void merge(std::vector<char*>& files, FILE* dest) {
    for(std::size_t i = 0; i < files.size(); ++i)
        fwrite(files[i], sizeof(char), strlen(files[i]), dest);
}
