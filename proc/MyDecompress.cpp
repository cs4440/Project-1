#include <fcntl.h>   // constants, O_RDONLY, O_RDWR, etc
#include <stdio.h>   // fopen()
#include <unistd.h>  // open(), read(), write()
#include <iostream>

// decompress a compressed file of 0/1
// @param src - reference input file stream
// @param dest - reference output file stream
void decompress(FILE *src, FILE *dest);

// expand n number of bits at end of destination file
// @param dest - output file stream to write
// @param c - character to write
// @param size - number of characers to write
void expand_n_bits(FILE *dest, char c, int size);

int main(int argc, char *argv[]) {
    char default_src[] = "res/compress.txt",
         default_dest[] = "res/decompress.txt";
    char *src = default_src, *dest = default_dest;

    // check for argument overrides
    if(argc > 1) src = argv[1];
    if(argc > 2) dest = argv[2];

    FILE *fsrc = fopen(src, "rb"),   // input file
        *fdest = fopen(dest, "wb");  // output file

    if(!fsrc || !fdest)
        std::cerr << "fopen() failed" << std::endl;
    else {
        std::cout << "Decompressing text: src(" << src << ") dest(" << dest
                  << ")" << std::endl;
        decompress(fsrc, fdest);
        std::cout << "Decompression complete" << std::endl;
    }

    if(fsrc) fclose(fsrc);
    if(fdest) fclose(fdest);

    return 0;
}

void decompress(FILE *src, FILE *dest) {
    int chr;  // character to get from src
    std::string sign;

    while((chr = fgetc(src)) != EOF) {
        if(chr == '-' || chr == '+') {
            sign.clear();
            while((chr = fgetc(src)) != '-' && chr != '+') sign += chr;

            chr = chr == '-' ? '0' : '1';
            fputs(std::string(atoi(sign.c_str()), chr).c_str(), dest);
        } else
            fputc(chr, dest);
    }
}
