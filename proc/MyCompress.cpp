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
    char default_src[] = "res/sample.txt", default_dest[] = "res/compress.txt";
    char *src = default_src, *dest = default_dest;

    // check for argument overrides
    if(argc > 1) src = argv[1];
    if(argc > 2) dest = argv[2];

    FILE *fsrc = fopen(src, "rb"),   // input file
        *fdest = fopen(dest, "wb");  // output file

    if(!fsrc || !fdest)
        std::cerr << "fopen() failed" << std::endl;
    else {
        std::cout << "Compressing text: src(" << src << ") dest(" << dest << ")"
                  << std::endl;
        compress(fsrc, fdest);
        std::cout << "Compression complete" << std::endl;
    }

    if(fsrc) fclose(fsrc);
    if(fdest) fclose(fdest);

    return 0;
}

void compress(FILE *src, FILE *dest) {
    int chr,         // character to get from src
        count = 0,   // count the number of same characters
        limit = 16,  // limit for compression
        peek;        // next character

    while((chr = fgetc(src)) != EOF) {
        ++count;            // count number of characters read
        peek = fgetc(src);  // peek at next char
        ungetc(peek, src);  // return peek character

        if(chr != peek) {  // if peek char is diff from current, write
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
        if(c == '0')
            c = '-';
        else if(c == '1')
            c = '+';

        std::string str(std::string(&c) + std::to_string(count) +
                        std::string(&c));
        fputs(str.c_str(), dest);
    }
}
