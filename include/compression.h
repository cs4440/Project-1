#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdio.h>  // FILE*
#include <string>   // string

// compress ascii 0 and 1 text file
// @param src - reference input file stream
// @param dest - reference output file stream
void compress(FILE *src, FILE *dest);

// decompress a compressed file of 0/1
// @param src - reference input file stream
// @param dest - reference output file stream
void decompress(FILE *src, FILE *dest);

// write n number of bits to end of file
// @param dest - output file stream to write
// @param c - character to write
// @param size - number of characers to write
// @param limit - integer limit to switch to compression symbol; otherwise
//                just write n number of bits to file
void write_compression(FILE *dest, char c, int count, int limit);

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
            write_compression(dest, chr, count, limit);
            count = 0;
        }
    }
}

void write_compression(FILE *dest, char c, int count, int limit) {
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

void decompress(FILE *src, FILE *dest) {
    int chr,  // character to get from src
        n;    // sign count
    std::string sign;

    while((chr = fgetc(src)) != EOF) {
        if(chr == '-' || chr == '+') {
            sign.clear();
            while((chr = fgetc(src)) != '-' && chr != '+') sign += chr;
            n = atoi(sign.c_str());

            chr = chr == '-' ? '0' : '1';
            for(int i = 0; i < n; ++i) fputc(chr, dest);
        } else
            fputc(chr, dest);
    }
}

#endif  // COMPRESSION_H