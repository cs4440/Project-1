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
    if(argc < 3)
        std::cout << "Insufficient arguments" << std::endl;
    else {
        std::cout << "Deompressing text: src(" << argv[1] << ") dest("
                  << argv[2] << ")" << std::endl;

        int fd_src = open(argv[1], O_RDONLY);
        int fd_dest = open(argv[2], O_RDWR | O_CREAT);
        FILE *src = fdopen(fd_src, "rb"),    // input file
            *dest = fdopen(fd_dest, "w+b");  // output file

        if(!src || !dest)
            std::cerr << "fopen() failed" << std::endl;
        else
            decompress(src, dest);

        std::cout << "Decompression complete." << std::endl;

        fclose(src);
        fclose(dest);
    }

    return 0;
}

void decompress(FILE *src, FILE *dest) {
    bool sign_flag = false;
    int chr,        // character to get from src
        count = 0,  // count the number of same characters
        peak;       // next character

    while((chr = fgetc(src)) != EOF) {
        ++count;            // count number of characters read
        peak = fgetc(src);  // peak at next char
        ungetc(peak, src);  // return peak character

        if(chr == '-' || chr == '+') {  // if currrent char is sign
            sign_flag = !sign_flag;     // flag it, reset counter
            count = 0;                  // skip to next loop
            continue;
        }

        if(!sign_flag && chr != peak) {       // if sign flag is false
            expand_n_bits(dest, chr, count);  // and next char is not same
            count = 0;                        // write normal bits
        }

        // if sign flag is true and peek char is closing sign
        // then grab ascii number and expand on number
        if(sign_flag && (peak == '-' || peak == '+')) {
            // grab ascii number to buffer and convert to int
            char *buf = new char[count + 2];
            fseek(src, -count, SEEK_CUR);
            fgets(buf, count + 1, src);
            buf[count] = '\0';

            expand_n_bits(dest, peak, atoi(buf));
            delete[] buf;
            count = 0;
        }
    }
}

void expand_n_bits(FILE *dest, char c, int size) {
    fseek(dest, 0, SEEK_END);  // seek write to end of file

    if(c == '-')
        c = '0';
    else if(c == '+')
        c = '1';

    for(int i = 0; i < size; ++i) fputc(c, dest);
}
