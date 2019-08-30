#include <fstream>
#include <iostream>
#include <string>

// compress ascii 0 and 1 text file
// @param src - reference input file stream
// @param dest - reference output file stream
void compress(std::fstream &src, std::fstream &dest);

// compress n number of bits to end of file
// @param dest - output file stream to write
// @param c - character to write
// @param count - number of characers to write
// @param limit - integer limit to switch to compression symbol; otherwise
//                just write n number of bits to file
void compress_to_file(std::fstream &dest, char c, int count, int limit);

int main(int argc, char *argv[]) {
    if(argc < 3)
        std::cout << "Insufficient arguments" << std::endl;
    else {
        std::cout << "Compressing text: src(" << argv[1] << ") dest(" << argv[2]
                  << ")" << std::endl;

        std::fstream fin(argv[1], std::ios::in | std::ios::binary);
        std::fstream fout(argv[2],
                          std::ios::in | std::ios::out | std::ios::binary);

        if(!fin || !fout)
            std::cerr << "fstream failed" << std::endl;
        else
            compress(fin, fout);

        std::cout << "Compression complete." << std::endl;

        fin.close();
        fout.close();
    }

    return 0;
}

void compress(std::fstream &src, std::fstream &dest) {
    int count = 0, limit = 16;
    char curr = '\0';

    while(src.get(curr)) {
        ++count;

        if(curr != src.peek()) {
            compress_to_file(dest, curr, count, limit);
            count = 0;
        }
    }
}

void compress_to_file(std::fstream &dest, char c, int count, int limit) {
    dest.seekp(0, dest.end);  // seek write to end of file

    if(count < limit) {
        std::string str(count, c);
        dest.write(str.c_str(), count);
    } else {
        std::string str;

        if(c == '0')
            str = "-" + std::to_string(count) + "-";
        else
            str = "+" + std::to_string(count) + "+";

        dest.write(str.c_str(), str.size());
    }
}
