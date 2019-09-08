/*
 * Compress binary file of 0's and 1's
 *
 * DETAILS
 * -------
 * If repeating 0 or 1 is less than 16, then simply write to file.
 * Otherwise, if greater than or equal to 16, then write -num- for
 * repeating 0's or +num+ for repeating 1's.
 */
#include <fcntl.h>   // constants, O_RDONLY, O_RDWR, etc
#include <unistd.h>  // open(), read(), write()
#include <cstring>   // strln()
#include <iostream>

// compress file
// @param fd_src - input file descriptor
// @param fd_dest - output file descriptor
void compress(int fd_src, int fd_dest);

// write compression to file by limit
// @param fd - file descriptor
// @param c - character to write
// @param count - number of characters to write
// @param limit - limit for compression
void compress_to_file(int fd, char c, int count, int limit);

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

    std::cout << "Compressing text: src(" << argv[1] << ") dest(" << argv[2]
              << ")" << std::endl;
    compress(fd_src, fd_dest);
    std::cout << "Compression complete." << std::endl;

    close(fd_src);
    close(fd_dest);

    return 0;
}

void compress(int fd_src, int fd_dest) {
    int bytes = 0, buf_sz = 100, count = 0, limit = 16;
    char cur = '\0';
    char *buf = new char[buf_sz + 1];  // +1 size for nul terminate

    while((bytes = read(fd_src, buf, buf_sz)) > 0) {
        buf[bytes] = '\0';

        for(int i = 0; i < bytes; ++i) {
            cur = buf[i];
            ++count;

            // if cur char is not same as next char and
            // next char is not NULL, write to file
            // else count is preserved for next block
            if(cur != buf[i + 1] && buf[i + 1] != '\0') {
                compress_to_file(fd_dest, buf[i], count, limit);
                count = 0;
            }
        }
    }

    // write leftover chars
    if(count > 0) compress_to_file(fd_dest, cur, count, limit);

    delete[] buf;
}

void compress_to_file(int fd, char c, int count, int limit) {
    char *buf = NULL;
    int len = 0;
    lseek(fd, 0, SEEK_END);  // seek to end of file

    if(count < limit) {
        buf = new char[count];

        for(int i = 0; i < count; ++i) buf[i] = c;
        write(fd, buf, count);

        delete[] buf;
    } else {
        buf = new char[25];
        c = c == '0' ? '-' : '+';  // find compression syymbl

        buf[0] = c;                     // add first symbol
        sprintf(buf + 1, "%d", count);  // convert int to cstring on pos 1
        buf[len = strlen(buf)] = c;     // add 2nd symbol at end
        write(fd, buf, len + 1);

        delete[] buf;
    }
}
