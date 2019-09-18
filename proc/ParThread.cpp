/*
 * Concurrency: calls compress() n times via pthreads
 *
 * DETAILS
 * -------
 * Splits file by n times. Calls compress() to work on each split file
 * via pthreads. Then merge all the compression results to final output.
 */
#include <pthread.h>                 // pthread, pthread_t
#include <sys/stat.h>                // stat(), mkdir()
#include <unistd.h>                  // ftruncate()
#include <cassert>                   // assert()
#include <cstdio>                    // fopen()
#include <cstdlib>                   // atoi()
#include <iostream>                  // stream
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
         default_dest[] = "res/parcompress.txt";
    char *src = default_src, *dest = default_dest;
    // FILE* files[2];      // array of FILE * for pthread_create
    FILE **files = nullptr, **cur_files = nullptr;
    unsigned n = 3;      // default n split
    struct stat buffer;  // directory status info
    pthread_t* tid;      // point to tid array
    std::vector<std::string> split_files, compressed_files;

    // check for argument overrides
    if(argc > 1) src = argv[1];
    if(argc > 2) dest = argv[2];
    if(argc > 3) n = atoi(argv[3]);

    // check if directory tmp/ exists
    // stat() returns 0 if exists
    if(stat("tmp/", &buffer) != 0) mkdir("tmp/", 0700);

    // create temporary split file names to tmp/ dir
    for(unsigned i = 0; i < n; ++i)
        split_files.emplace_back("tmp/" + std::to_string(i) + ".split");

    // create tmp compresed files names
    for(const auto& file : split_files)
        compressed_files.emplace_back(file + std::string(".cmp"));

    split_file(src, n, split_files);

    std::cout << "Starting pthread concurrency compression of " << n
              << std::endl;

    // create pthread_t array and FILE* array
    tid = new pthread_t[n];
    cur_files = files = new FILE*[2 * n];

    // spawn threads to do compress
    for(unsigned i = 0; i < n; ++i) {
        cur_files[0] = fopen(split_files[i].c_str(), "rb");       // src
        cur_files[1] = fopen(compressed_files[i].c_str(), "wb");  // dest

        pthread_create(&tid[i], nullptr, compress, cur_files);

        cur_files += 2;  // calculate new files array address x2
    }
    for(unsigned i = 0; i < n; ++i) pthread_join(tid[i], nullptr);
    for(unsigned i = 0; i < 2 * n; ++i) fclose(files[i]);

    merge(compressed_files, dest);

    std::cout << "Concurrency compression complete" << std::endl;

    delete[] files;
    delete[] tid;

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
