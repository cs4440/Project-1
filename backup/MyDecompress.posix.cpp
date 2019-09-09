/*
 * Decompress file generated by MyCompress program
 *
 * DETAILS
 * -------
 * Decompressing symbols of '-' is 0 and '+' is 1.
 * Examples:
 * -16- --> 0000000000000000
 * +20+ --> 11111111111111111111
 */
#include <fcntl.h>   // constants, O_RDONLY, O_RDWR, etc
#include <unistd.h>  // open(), read(), write()
#include <iostream>
#include <string>

// decompress file
// @param fd_src - input file descriptor
// @param fd_dest - output file descriptor
void decompress(int fd_src, int fd_dest);

// write compression to file by limit
// @param fd - file descriptor
// @param c - character to write
// @param count - number of characters to write
// @param limit - limit for compression
void expand_to_file(int fd, char c, int count);

int main(int argc, char *argv[]) {
    int fd_src = open(argv[1], O_RDONLY);
    int fd_dest = open(argv[2], O_RDWR | O_CREAT | O_TRUNC);

    if(argc < 3) {
        std::cerr << "Insufficient arguments." << std::endl;
        return 1;
    }

    if(fd_src < 0 || fd_dest < 0) {
        std::cerr << "open() files failed." << std::endl;
        close(fd_src);
        close(fd_dest);

        return 1;
    }

    std::cout << "Decompressing text: src(" << argv[1] << ") dest(" << argv[2]
              << ")" << std::endl;
    decompress(fd_src, fd_dest);
    std::cout << "Decompression complete." << std::endl;

    close(fd_src);
    close(fd_dest);

    return 0;
}

void decompress(int fd_src, int fd_dest) {
    bool sign_flag = false;
    int buf_sz = 100, bytes = 0, count = 0;
    char cur = '\0', carry_over = '\0';
    char *buf = new char[buf_sz + 1];  // +1 size for nul terminate
    std::string sign_count;

    while((bytes = read(fd_src, buf, buf_sz)) > 0) {
        buf[bytes] = '\0';

        for(int i = 0; i < bytes; ++i) {
            cur = buf[i];

            if(cur == '-' || cur == '+') {  // if currrent char is sign
                sign_flag = !sign_flag;     // flag it, reset counter
                count = 0;                  // skip to next loop
                continue;
            }

            // check if previous block has any carry over characters
            if(carry_over != '\0' && carry_over != cur) {
                expand_to_file(fd_dest, carry_over, count);
                carry_over = '\0';
                count = 1;
            } else
                ++count;

            // if sign_flag, accum string with num char
            // else clear string
            sign_count = sign_flag ? sign_count + cur : "";

            // if !sign_flag and next char isn't same, write to file
            // exept if last block to preesrve count for next read
            if(!sign_flag && cur != buf[i + 1] && buf[i + 1] != '\0') {
                expand_to_file(fd_dest, buf[i], count);
                count = 0;
            }

            // if sign flag is true and next char is compress symbol
            // then write with sign_count
            if(sign_flag && (buf[i + 1] == '-' || buf[i + 1] == '+')) {
                expand_to_file(fd_dest, buf[i + 1], atoi(sign_count.c_str()));
                count = 0;
            }

            // set carry_over if last block has characters left
            if(buf[i + 1] == '\0' && count > 0) carry_over = cur;
        }
    }
    // write leftover chars
    if(count > 0) expand_to_file(fd_dest, cur, count);

    delete[] buf;
}

void expand_to_file(int fd, char c, int count) {
    char *buf = new char[count];

    // check for compression symbol
    if(c == '-')
        c = '0';
    else if(c == '+')
        c = '1';

    lseek(fd, 0, SEEK_END);  // seek to end of file
    for(int i = 0; i < count; ++i) buf[i] = c;
    write(fd, buf, count);

    delete[] buf;
}
