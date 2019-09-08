/*
 * Concurrency: calls MyCompress n times to work on large file via fork()
 *
 * DETAILS
 * -------
 * Splits file by n times. Calls MyCompress to work on each split file
 * via fork(). Then merge all the compression results to final output.
 */
#include <fcntl.h>      // constants, O_RDONLY, O_RDWR, etc
#include <sys/stat.h>   // stat(), mkdir()
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait()
#include <unistd.h>     // open(), bytes(), write(), lseek(), fork(), exec(),
#include <cassert>      // asser()
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

// copy fd_src file to fd_dest file by n total_bytes size
// @param fd_src - input file
// @param fd_dest - output file
// @param size - number of total_bytes to copy
void readwrite_n(int fd_src, int fd_dest, long int size);

int main(int argc, char* argv[]) {
    char default_src[] = "res/sample.txt", default_dest[] = "res/compress.txt";
    char *fd_src = default_src, *fd_dest = default_dest;
    unsigned n = 3;                        // number of n splits
    struct stat buffer;                    // check for path info
    std::vector<std::string> split_files,  // names for splitting files
        compressed_files;                  // names for split compression files

    // check for argument overrides
    if(argc > 2) fd_src = argv[1];
    if(argc > 3) fd_dest = argv[2];
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

    split_file(fd_src, n, split_files);

    std::cout << "Starting concurrency compression of " << n << "."
              << std::endl;

    for(unsigned i = 0; i < n; ++i)
        if(fork() == 0) {
            execle("MyCompress", "MyCompress", split_files[i].c_str(),
                   compressed_files[i].c_str(), nullptr);

            // exec fails here
            std::cout << "Exec error." << std::endl;
            return 1;
        }
    for(unsigned i = 0; i < n; ++i) wait(nullptr);

    std::cout << "Concurrency compression complete." << std::endl;

    // TODO
    // change MyCompress to merge all files
    // MyCompress need to also scan compression symbols to merge
    // between last -15- on first file and -21- first on second file

    return 0;
}

bool split_file(char* name, unsigned n, std::vector<std::string>& files) {
    bool status = true;
    long int file_size = 0, split_size = 0;
    int fd_src = -1;

    // check files have n output files names
    assert(n == files.size());

    fd_src = open(name, O_RDONLY);

    if(fd_src < 0) {
        std::cerr << "open() files failed." << std::endl;
        status = false;
    } else {
        // calculate total file size and split size
        file_size = lseek(fd_src, 0, SEEK_END);
        split_size = file_size / n;
        lseek(fd_src, 0, SEEK_SET);

        if(file_size)
            for(unsigned i = 0; i < n; ++i) {
                // on last file, recalc split size from integer division loss
                if(i == n - 1)
                    split_size = file_size - lseek(fd_src, 0, SEEK_CUR);

                int tmp = open(files[i].c_str(), O_RDWR | O_CREAT | O_TRUNC);
                readwrite_n(fd_src, tmp, split_size);
                close(tmp);
            }
        else
            status = false;
    }
    close(fd_src);

    return status;
}

void readwrite_n(int fd_src, int fd_dest, long int limit) {
    char* buf = nullptr;
    long int total_bytes = limit;
    int buf_sz = 30, bytes = 0;

    // check for buffer size and bytes limit difference
    if(limit < buf_sz) buf_sz = limit;

    buf = new char[buf_sz + 1];

    while(total_bytes > 0 && (bytes = read(fd_src, buf, buf_sz)) > 0) {
        buf[bytes] = '\0';
        write(fd_dest, buf, bytes);

        total_bytes -= bytes;
        if(total_bytes < buf_sz) buf_sz = total_bytes;
    }

    delete[] buf;
}
