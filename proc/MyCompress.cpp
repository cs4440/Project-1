#include <fcntl.h>                   // constants, O_RDONLY, O_RDWR, etc
#include <stdio.h>                   // fopen()
#include <iostream>                  // stream
#include <string>                    // string
#include "../include/compression.h"  // compress()

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
