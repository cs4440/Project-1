/*
 * Comcurrency n calls of MyCompress working on large file via fork()
 */
#include <sys/stat.h>   // stat(), mkdir()
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait()
#include <unistd.h>     // exec()
#include <unistd.h>     // fork()
#include <cassert>      // asser()
#include <cstdio>       // fopen()
#include <cstdlib>      // atoi()
#include <iostream>     // stream
#include <vector>       // vector

// splits a file by n times to files
// precondition: files must have n output file names
// @param name - input file
// @param n - split by n times
// @param files - list of output file names
// return success state
bool split_file(char* name, unsigned n, std::vector<std::string>& files);

// copy src file to dest file by n bytes size
// @param src - input file
// @param dest - output file
// @param size - number of bytes to copy
// return number of bytes read/written
long int readwrite_n(FILE* src, FILE* dest, long int size);

int main(int argc, char* argv[]) {
    char default_src[] = "res/sample.txt", default_dest[] = "res/compress.txt";
    char *src = default_src, *dest = default_dest;
    unsigned n = 3;
    struct stat buffer;
    std::vector<std::string> split_files, compressed_files;

    // check for argument overrides
    if(argc > 2) src = argv[1];
    if(argc > 3) dest = argv[2];
    if(argc > 4) n = atoi(argv[3]);

    // check if directory tmp/ exists
    // stat() returns 0 if exists
    if(stat("tmp/", &buffer) != 0) mkdir("tmp/", 0700);

    // create temporary split file names to tmp/ dir
    for(unsigned i = 0; i < n; ++i)
        split_files.emplace_back("tmp/" + std::to_string(i) + ".split");

    // create tmp compresed files names
    for(const auto& file : split_files)
        compressed_files.emplace_back(file + std::string(".cmp"));

    split_file(src, 3, split_files);

    std::cout << "Starting concurrency compression of " << n << "."
              << std::endl;

    for(unsigned i = 0; i < n; ++i)
        if(fork() == 0) {
            execle("MyCompress", "MyCompress", split_files[i].c_str(),
                   compressed_files[i].c_str(), nullptr);
            return 0;
        }
    for(unsigned i = 0; i < n; ++i) wait(nullptr);

    std::cout << "Concurrency ompression times complete." << std::endl;

    // TODO
    // change MyCompress to merge all files
    // MyCompress need to also scan compression symbols to merge
    // between -15- on first file and -21- on second file

    return 0;
}

bool split_file(char* name, unsigned n, std::vector<std::string>& files) {
    bool status = true;
    long int file_size = 0, split_size = 0;
    FILE* src = nullptr;

    // check files have n output files names
    assert(n == files.size());

    src = fopen(name, "rb");

    if(src) {
        // calculate total file size and split size
        fseek(src, 0, SEEK_END);
        file_size = ftell(src);
        split_size = file_size / n;
        fseek(src, 0, SEEK_SET);

        if(file_size)
            for(unsigned i = 0; i < n; ++i) {
                // on last file, recal split size from integer division loss
                if(i == n - 1) {
                    int tell = ftell(src);
                    split_size = file_size - tell;
                    fseek(src, tell, SEEK_SET);
                }

                FILE* tmp = fopen(files[i].c_str(), "w+b");
                readwrite_n(src, tmp, split_size);
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

long int readwrite_n(FILE* src, FILE* dest, long int limit) {
    char* buf = nullptr;
    int bytes = limit;
    long int buf_sz = 30, read = 0, total_read = 0;

    // check for buffer size and read limit difference
    if(limit < buf_sz) buf_sz = limit;

    buf = new char[buf_sz + 1];

    while(bytes > 0 && (read = fread(buf, 1, buf_sz, src)) > 0) {
        buf[read] = '\0';
        fwrite(buf, 1, read, dest);

        bytes -= read;
        if(bytes < buf_sz) buf_sz = bytes;
    }

    delete[] buf;

    return total_read;
}
