/*
 * Concurrency: calls compress() n times via threads
 *
 * DETAILS
 * -------
 * Splits file by n times. Calls compress() to work on each split file
 * via std::thread. Then merge all the compression results to final output.
 */
#include <sys/stat.h>                // stat(), mkdir()
#include <cstdio>                    // fopen()
#include <cstdlib>                   // atoi()
#include <cstring>                   // strlen()
#include <iostream>                  // stream
#include <thread>                    // thread
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
         default_dest[] = "res/par_thread_compress.txt";
    char *src = default_src, *dest = default_dest;
    char* src_arr = nullptr;           // array to hold source content
    unsigned n = 3;                    // default n split
    struct stat buffer;                // directory status info
    FILE *fsrc = nullptr,              // source FILE*
        *fdest = nullptr;              // dest FILE*
    long int file_size = 0,            // total file size
        split_size = 0;                // split file size
    std::vector<std::thread> threads;  // vector of threads
    std::vector<char*> splits;         // resultant array for compression
    timer::ChronoTimer ct;             // chrono timer

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
        fclose(fsrc);
        return 1;
    } else if(file_size < n) {
        std::cerr << "Input file is smaller than n concurrency" << std::endl;
        fclose(fsrc);
        return 1;
    }

    std::cout << "Starting std::thread concurrency compression of " << n
              << std::endl;

    // start timer
    ct.start();

    // read file to array
    src_arr = new char[file_size + 1];
    read_to_arr(fsrc, src_arr);

    for(unsigned i = 0; i < n; ++i) {
        // on last file, recalc split size from integer division loss
        if(i == n - 1) split_size = (file_size - (split_size * i));

        splits.emplace_back(new char[split_size + 1]);

        // start a thread to compress split files
        threads.emplace_back(compress_arr, src_arr + (split_size * i),
                             splits[i], split_size);
    }
    for(std::size_t i = 0; i < threads.size(); ++i) threads[i].join();

    // open output file and merge all compressed_files to output
    fdest = fopen(dest, "w+b");
    merge(splits, fdest);

    // stop timer
    ct.stop();

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
