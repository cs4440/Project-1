/*
 * Concurrency: calls MyCompress n times to work on large file via fork()
 *
 * DETAILS
 * -------
 * Splits file by n times. Calls MyCompress to work on each split file
 * via fork(). Then merge all the compression results to final output.
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
// @param input - input file name
// @param n - split by n times
// @param files - list of output file names
// return success state
bool split_file(char* input, unsigned n, std::vector<std::string>& files);

// merges files and fixes beginning/end of each files
// @param files - a vector of n input file names
// @param output - output file name
void merge(std::vector<std::string>& files, char* output);

// compress n number of bits to end of file
// @param dest - output file stream to write
// @param c - character to write
// @param size - number of characers to write
// @param limit - integer limit to switch to compression symbol; otherwise
//                just write n number of bits to file
void compress_to_file(FILE* dest, char c, int count, int limit);

int main(int argc, char* argv[]) {
    char default_src[] = "res/sample.txt",
         default_dest[] = "res/parcompress.txt";
    char *src = default_src, *dest = default_dest;
    unsigned n = 3;
    pid_t pid = -1;
    struct stat buffer;
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

    std::cout << "Starting concurrency compression of " << n << std::endl;

    for(unsigned i = 0; i < n; ++i)
        if((pid = fork()) == 0) {
            execle("MyCompress", "MyCompress", split_files[i].c_str(),
                   compressed_files[i].c_str(), nullptr);

            // exec fails here
            std::cerr << "exec() failed" << std::endl;
            return 1;
        } else if(pid < 0) {
            std::cerr << "fork() failed" << std::endl;
            return 1;
        }
    for(unsigned i = 0; i < n; ++i) wait(nullptr);

    merge(compressed_files, dest);

    std::cout << "Concurrency compression complete" << std::endl;

    return 0;
}

bool split_file(char* input, unsigned n, std::vector<std::string>& files) {
    bool status = true;
    int chr;
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
                    int tell = ftell(src);
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
    bool sign_flag = false, append = false;
    int chr = -1,        // character to get from src
        count = 0,       // count the number of same characters
        limit = 16,      // limit for compression
        peek = -1,       // next character
        prev = -1,       // previous character
        prev_count = 0,  // previous count
        n = 0;           // number from sign compression
    FILE *dest = fopen(output, "wb"), *src = nullptr, *src_peek = nullptr;
    std::string sign;

    for(std::size_t i = 0; i < files.size(); ++i) {
        src = fopen(files[i].c_str(), "rb");

        while((chr = fgetc(src)) != EOF) {
            ++count;
            peek = fgetc(src);  // peek at next char
            ungetc(peek, src);  // return peek character

            if(chr == '-' || chr == '+') {
                prev = chr == '-' ? '0' : '1';
                sign_flag = true;

                // get symbol string
                sign.clear();
                while((chr = fgetc(src)) != '-' && chr != '+') sign += chr;
                n = atoi(sign.c_str());

                // if prev block has append, then add count to sign string
                if(append) {
                    sign = std::string(std::to_string(n + count - 1));
                    append = false;
                }

                sign = std::string(1, (char)chr) + sign +
                       std::string(1, (char)chr);
                fwrite(sign.c_str(), 1, sign.size(), dest);

                count = 0;
            } else if(chr != peek) {
                sign_flag = false;
                compress_to_file(dest, chr, count, limit);

                if(peek == EOF) {
                    prev = chr;
                    prev_count = count;
                }
                count = 0;
            }
        }
        // peek at next file's char and see if same as prev
        if(i != files.size() - 1) {
            src_peek = fopen(files[i + 1].c_str(), "rb");
            peek = fgetc(src_peek);

            // determine corresponding bytes from symbol
            if(peek == '-') peek = '0';
            if(peek == '+') peek = '1';

            // if peek is same as prev then,
            // determine new count, dest fseek backwards, and truncate
            if(prev == peek) {
                append = true;

                if(sign_flag) {
                    count = n;
                    fseek(dest, -sign.size(), SEEK_END);
                } else {
                    count = prev_count;
                    fseek(dest, -count, SEEK_END);
                }
                ftruncate(fileno(dest), ftell(dest));
            }
            fclose(src_peek);
        }
    }
    fclose(dest);
}

void compress_to_file(FILE* dest, char c, int count, int limit) {
    fseek(dest, 0, SEEK_END);  // seek to end of file

    if(count < limit) {
        std::string str(count, c);
        fputs(str.c_str(), dest);
    } else {
        std::string str;

        if(c == '0')
            c = '-';
        else if(c == '1')
            c = '+';

        str = std::string(&c) + std::to_string(count) + std::string(&c);

        fputs(str.c_str(), dest);
    }
}
