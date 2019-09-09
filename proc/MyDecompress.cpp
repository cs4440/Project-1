#include <fcntl.h>                   // constants, O_RDONLY, O_RDWR, etc
#include <stdio.h>                   // fopen()
#include <iostream>                  // stream
#include "../include/compression.h"  // decompress()

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
