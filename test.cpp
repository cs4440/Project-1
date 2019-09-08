#include <fcntl.h>   // constants, O_RDONLY, O_RDWR, etc
#include <unistd.h>  // open(), bytes(), write(), lseek(), fork(), exec(),
#include <iostream>

//

int main() {
    int fd = open("samp.txt", O_RDONLY);

    int buf_sz = 24, bytes = 0, count = 0, limit = 0;
    char cur = '\0';
    char *buf = new char[buf_sz + 1];

    while((bytes = read(fd, buf, buf_sz)) > 0) {
        buf[bytes] = '\0';

        for(int i = 0; i < bytes; ++i) {
            cur = buf[i];
            ++count;

            //
        }
    }
}
