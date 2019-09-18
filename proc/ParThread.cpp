/*
 * Concurrency: calls compress() n times via threads
 *
 * DETAILS
 * -------
 * Splits file by n times. Calls compress() to work on each split file
 * via std::thread. Then merge all the compression results to final output.
 */
#include <sys/stat.h>                // stat(), mkdir()
#include <cassert>                   // assert()
#include <cstdio>                    // fopen()
#include <cstdlib>                   // atoi()
#include <iostream>                  // stream
#include <thread>                    // thread
#include <vector>                    // vector
#include "../include/compression.h"  // compress()

// splits a file by n times to files
// precondition: files must have n output file names
// @param input - input file name
// @param n - split by n times
// @param files - list of output file names
// return success state
bool split_file(char* input, unsigned n, std::vector<std::string>& files);

// merges files without fixing inconsistence truncations
// @param files - a vector of n input file names
// @param output - output file name
void merge(std::vector<std::string>& files, char* output);

int main(int argc, char* argv[]) {
    char default_src[] = "res/sample.txt",
         default_dest[] = "res/par_thread_compress.txt";
    char *src = default_src, *dest = default_dest;
    unsigned n = 3;                        // default n split
    struct stat buffer;                    // directory status info
    std::vector<std::string> split_files,  // split file names
        compressed_files;                  // names for split compress result
    std::vector<std::thread> threads;      // vector of threads
    std::vector<FILE*> files;              // vector of files for src and dest

    // check for argument overrides
    if(argc > 1) src = argv[1];
    if(argc > 2) dest = argv[2];
    if(argc > 3) n = atoi(argv[3]);

    // check if directory tmp/ exists, stat() returns 0 if exists
    if(stat("tmp/", &buffer) != 0) mkdir("tmp/", 0700);

    // create temporary split file names to tmp/ dir
    for(unsigned i = 0; i < n; ++i)
        split_files.emplace_back("tmp/" + std::to_string(i) + ".split");

    // create tmp compresed files names
    for(const auto& file : split_files)
        compressed_files.emplace_back(file + std::string(".cmp"));

    split_file(src, n, split_files);

    std::cout << "Starting std::thread concurrency compression of " << n
              << std::endl;

    // spawn threads to do compress and push each thread to vector
    for(unsigned i = 0; i < n; ++i) {
        // open src and dest FILE* for compression
        files.push_back(fopen(split_files[i].c_str(), "rb"));       // src
        files.push_back(fopen(compressed_files[i].c_str(), "wb"));  // dest

        // directly create thread via emplace_back
        threads.emplace_back(compress, files[2 * i + 0], files[2 * i + 1]);
    }
    for(unsigned i = 0; i < threads.size(); ++i) threads[i].join();
    for(unsigned i = 0; i < files.size(); ++i) fclose(files[i]);

    merge(compressed_files, dest);

    std::cout << "Concurrency compression complete" << std::endl;

    return 0;
}

bool split_file(char* input, unsigned n, std::vector<std::string>& files) {
    bool status = true;
    int chr, tell;
    long int file_size = 0, split_size = 0;
    FILE* src = nullptr;

    // check files have n output files names
    assert(n == files.size());

    src = fopen(input, "rb");

    if(src) {
        // calculate total file size and split size
        fseek(src, 0, SEEK_END);
        file_size = ftell(src);
        split_size = file_size / n;
        fseek(src, 0, SEEK_SET);

        if(file_size)
            for(unsigned i = 0; i < n; ++i) {
                // on last file, recalc split size from integer division loss
                if(i == n - 1) {
                    tell = ftell(src);
                    split_size = file_size - tell;
                    fseek(src, tell, SEEK_SET);
                }

                FILE* tmp = fopen(files[i].c_str(), "w+b");
                for(long int i = 0; i < split_size; ++i)
                    if((chr = fgetc(src)) != EOF) fputc(chr, tmp);
                fclose(tmp);
            }
        else
            status = false;

    } else {
        std::cout << "File open failed" << std::endl;
        status = false;
    }
    fclose(src);

    return status;
}

void merge(std::vector<std::string>& files, char* output) {
    int chr;
    FILE *src = nullptr, *dest = fopen(output, "wb");

    for(std::size_t i = 0; i < files.size(); ++i) {
        src = fopen(files[i].c_str(), "rb");
        while((chr = fgetc(src)) != EOF) fputc(chr, dest);
        fclose(src);
    }
    fclose(dest);
}
