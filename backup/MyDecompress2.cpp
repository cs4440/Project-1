#include <fstream>
#include <iostream>

// decompress a compressed file of 0/1
// @param src - reference input file stream
// @param dest - reference output file stream
void decompress(std::fstream &src, std::fstream &dest);

// expand n number of bits at end of destination file
// @param dest - output file stream to write
// @param c - character to write
// @param size - number of characers to write
void expand_n_bits(std::fstream &dest, char c, int size);

int main(int argc, char *argv[]) {
    if(argc < 3)
        std::cout << "Insufficient arguments" << std::endl;
    else {
        std::cout << "Deompressing text: src(" << argv[1] << ") dest("
                  << argv[2] << ")" << std::endl;

        std::fstream fin(argv[1], std::ios::in | std::ios::binary);
        std::fstream fout(argv[2],
                          std::ios::in | std::ios::out | std::ios::binary);

        if(!fin || !fout)
            std::cerr << "fstream failed" << std::endl;
        else
            decompress(fin, fout);

        std::cout << "Decompression complete." << std::endl;

        fin.close();
        fout.close();
    }

    return 0;
}

void decompress(std::fstream &src, std::fstream &dest) {
    bool sign_flag = false;
    int count = 0;
    char curr = '\0';
    std::string sign_count;

    while(src.get(curr)) {
        ++count;

        if(curr == '-' || curr == '+') {  // if currrent char is sign
            sign_flag = !sign_flag;       // flag it, reset counter
            count = 0;                    // skip to next loop
            continue;
        }

        // if sign_flag, accum string with num char
        // else clear string
        sign_count = sign_flag ? sign_count + curr : "";

        if(!sign_flag && curr != src.peek()) {  // if sign flag is false
            expand_n_bits(dest, curr, count);   // and next char is not same
            count = 0;                          // write normal bits
        }

        // if sign flag is true and peek char is closing sign
        // then grab ascii number and expand on number
        if(sign_flag && (src.peek() == '-' || src.peek() == '+')) {
            expand_n_bits(dest, src.peek(), atoi(sign_count.c_str()));
            count = 0;
        }
    }
}

void expand_n_bits(std::fstream &dest, char c, int size) {
    dest.seekp(0, dest.end);  // seek write to end of file

    if(c == '-')
        c = '0';
    else if(c == '+')
        c = '1';

    for(int i = 0; i < size; ++i) dest.put(c);
}
