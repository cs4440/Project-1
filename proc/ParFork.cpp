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
#include <iostream>                  // stream
#include <vector>                    // vector
#include "../include/compression.h"  // compress()

// merges files without fixing inconsistence truncations
// @param files - a vector of n input file names
// @param output - output file name
void merge(std::vector<FILE*>& files, FILE* dest);

int main(int argc, char* argv[]) {
    char default_src[] = "res/sample.txt",
         default_dest[] = "res/par_fork_compress.txt";
    char *src = default_src, *dest = default_dest;
    unsigned n = 3;                  // default n split
    struct stat buffer;              // directory status info
    FILE *fsrc = nullptr,            // source FILE*
        *fdest = nullptr;            // dest FILE*
    int chr;                         // file character
    long int file_size = 0,          // total file size
        split_size = 0;              // split file size
    std::vector<FILE*> split_files,  // vector of split files
        compressed_files;            // vector of compressed files
    pid_t pid = -1;                  // process id

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
    split_size = file_size / n;
    fseek(fsrc, 0, SEEK_SET);

    if(!file_size) {
        std::cerr << "Input file is empty" << std::endl;
        return 1;
    }

    std::cout << "Starting std::thread concurrency compression of " << n
              << std::endl;

    for(unsigned i = 0; i < n; ++i) {
        std::string split = "tmp/" + std::to_string(i) + ".split",
                    post_split = split + ".cmp";

        // create split files and compressed files to vector
        split_files.push_back(fopen(split.c_str(), "w+b"));
        compressed_files.push_back(fopen(post_split.c_str(), "w+b"));

        // write src to split files
        for(long int j = 0; j < split_size; ++j)
            if((chr = fgetc(fsrc)) != EOF) fputc(chr, split_files[i]);

        // on last piece, write remaining bytes beyond integer division
        if(i == n - 1)
            while((chr = fgetc(fsrc)) != EOF) fputc(chr, split_files[i]);

        // rewind split files before passing to compress function
        rewind(split_files[i]);

        // fork a process to compress split files
        if((pid = fork()) == 0) {
            compress(split_files[i], compressed_files[i]);

            // close all files
            for(unsigned i = 0; i < split_files.size(); ++i) {
                fclose(split_files[i]);
                fclose(compressed_files[i]);
            }
            fclose(fsrc);

            return 0;
        }
    }
    for(unsigned i = 0; i < n; ++i) wait(nullptr);

    // open output file and merge all compressed_files to output
    fdest = fopen(dest, "w+b");
    merge(compressed_files, fdest);

    // close all files
    for(unsigned i = 0; i < split_files.size(); ++i) {
        fclose(split_files[i]);
        fclose(compressed_files[i]);
    }
    fclose(fdest);
    fclose(fsrc);

    std::cout << "Concurrency compression complete" << std::endl;

    return 0;
}

void merge(std::vector<FILE*>& files, FILE* dest) {
    int chr;

    for(std::size_t i = 0; i < files.size(); ++i) {
        rewind(files[i]);
        while((chr = fgetc(files[i])) != EOF) fputc(chr, dest);
    }
}
