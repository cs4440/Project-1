/*
 * Program to compare if two files are same.
 */
#include <iostream>  // stream

int main(int argc, char *argv[]) {
    if(argc < 3) {
        std::cerr << "Insufficient arguments" << std::endl;
        return 1;
    }

    bool is_same = true;
    int chr1, chr2;
    FILE *src = fopen(argv[1], "rb"), *target = fopen(argv[2], "rb");

    fseek(src, 0, SEEK_END);
    fseek(target, 0, SEEK_END);

    if(ftell(src) != ftell(target)) is_same = false;

    fseek(src, 0, SEEK_SET);
    fseek(target, 0, SEEK_SET);

    while((chr1 = fgetc(src)) != EOF) {
        chr2 = fgetc(target);

        if(chr1 != chr2) {
            is_same = false;
            break;
        }
    }

    if(is_same)
        std::cout << "Files are same" << std::endl;
    else
        std::cout << "Files are different" << std::endl;

    fclose(src);
    fclose(target);

    return 0;
}
