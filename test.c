#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/mydevice", O_RDONLY);
    if (fd == -1) {
        printf("Opening was not possible\n");
        return -1;
    }
    printf("Opening was successful\n");
    close(fd);
    return 0;
}