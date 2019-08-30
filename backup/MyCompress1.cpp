#include <fcntl.h>   // constants, O_RDONLY, O_RDWR, etc
#include <stdio.h>   // fopen()
#include <unistd.h>  // open(), read(), write()
#include <iostream>
#include <string>

// compress ascii 0 and 1 text file
// @param src - reference input file stream
// @param dest - reference output file stream
void compress(FILE *src, FILE *dest);

// compress n number of bits to end of file
// @param dest - output file stream to write
// @param c - character to write
// @param size - number of characers to write
// @param limit - integer limit to switch to compression symbol; otherwise
//                just write n number of bits to file
void compress_to_file(FILE *dest, char c, int count, int limit);

int main(int argc, char *argv[]) {
    if(argc < 3)
        std::cout << "Insufficient arguments" << std::endl;
    else {
        int fd_src = open(argv[1], O_RDONLY);
        int fd_dest = open(argv[2], O_RDWR | O_CREAT);
        FILE *src = fdopen(fd_src, "rb"),    // input file
            *dest = fdopen(fd_dest, "w+b");  // output file

        if(!src || !dest)
            std::cerr << "fopen() failed" << std::endl;
        else
            compress(src, dest);

        std::cout << "Compression complete." << std::endl;

        fclose(src);
        fclose(dest);
    }

    return 0;
}

void compress(FILE *src, FILE *dest) {
    int chr,         // character to get from src
        count = 0,   // count the number of same characters
        limit = 16,  // limit for compression
        peak;        // next character

    while((chr = fgetc(src)) != EOF) {
        ++count;            // count number of characters read
        peak = fgetc(src);  // peak at next char
        ungetc(peak, src);  // return peak character

        if(chr != peak) {  // if peak char is diff from current, write
            compress_to_file(dest, chr, count, limit);
            count = 0;
        }
    }
}

void compress_to_file(FILE *dest, char c, int count, int limit) {
    fseek(dest, 0, SEEK_END);  // seek to end of file

    if(count < limit) {
        std::string str(count, c);
        fputs(str.c_str(), dest);
    } else {
        std::string str;

        if(c == '0')
            str = "-" + std::to_string(count) + "-";
        else
            str = "+" + std::to_string(count) + "+";

        fputs(str.c_str(), dest);
    }
}
