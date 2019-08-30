/*
 * Deocompress file generated by MyCompress program
 */
#include <fcntl.h>   // constants, O_RDONLY, O_RDWR, etc
#include <unistd.h>  // open(), read(), write()
#include <iostream>
#include <string>

// decompress file
// @param fd_src - input file descriptor
// @param fd_dest - output file descriptor
void decompress(int fd_src, int fd_dest);

// read a block in file
// @param fd - file descriptor
// @param buf - read to buffer
// @param sz - size of buffer
// return number of bytes read
int read_block(int fd, char *buf, int sz);

// write compression to file by limit
// @param fd - file descriptor
// @param c - character to write
// @param count - number of characters to write
// @param limit - limit for compression
void expand_to_file(int fd, char c, int count);

int main(int argc, char *argv[]) {
    int fd_src = open(argv[1], O_RDONLY);
    int fd_dest = open(argv[2], O_RDWR | O_CREAT);

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
    int bytes_read = 0, buf_size = 100, count = 0;
    char cur = '\0';
    char *buf = new char[buf_size];
    std::string sign_count;

    while((bytes_read = read_block(fd_src, buf, buf_size)) > 0) {
        for(int i = 0; i < bytes_read; ++i) {
            cur = buf[i];
            ++count;

            if(cur == '-' || cur == '+') {  // if currrent char is sign
                sign_flag = !sign_flag;     // flag it, reset counter
                count = 0;                  // skip to next loop
                continue;
            }

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
        }
    }

    // write leftover chars
    if(count > 0) expand_to_file(fd_dest, cur, count);

    delete[] buf;
}

int read_block(int fd, char *buf, int sz) {
    int bytes_read = 0;

    bytes_read = read(fd, buf, sz);
    buf[bytes_read] = '\0';

    return bytes_read;
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
