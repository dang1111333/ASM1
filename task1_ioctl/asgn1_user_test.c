#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "asgn1_ioctl.h"

int main(int argc, char *argv[]) {
    int fd;

    if (argc < 2) {
        printf("Su dung: %s <reset | stats | setmode 0/1 | version>\n", argv[0]);
        return 1;
    }

    fd = open("/dev/asgn1", O_RDWR);
    if (fd < 0) {
        perror("Loi khong the mo /dev/asgn1");
        return 1;
    }

    if (strcmp(argv[1], "reset") == 0) {
        if (ioctl(fd, ASGN1_RESET_BUFFER) == 0) {
            printf("[IOCTL-USER] Reset buffer thanh cong!\n");
        } else {
            perror("[IOCTL-USER] Loi goi ASGN1_RESET_BUFFER");
        }
    } 
    else if (strcmp(argv[1], "stats") == 0) {
        struct asgn1_stats s;
        if (ioctl(fd, ASGN1_GET_STATS, &s) == 0) {
            printf("=== Thong ke hoat dong Driver ===\n");
            printf("  - So lan mo (open_count)       : %d\n", s.open_count);
            printf("  - So lan ghi (write_count)     : %d\n", s.write_count);
            printf("  - So lan doc (read_count)      : %d\n", s.read_count);
            printf("  - Do dai buffer (buffer_len)   : %d bytes\n", s.buffer_len);
            printf("  - Kich thuoc lan ghi cuoi      : %d bytes\n", s.last_write_size);
        } else {
            perror("[IOCTL-USER] Loi goi ASGN1_GET_STATS");
        }
    } 
    else if (strcmp(argv[1], "setmode") == 0) {
        if (argc < 3) {
            printf("Vui long nhap mode: %s setmode <0 hoac 1>\n", argv[0]);
            close(fd);
            return 1;
        }
        int mode = atoi(argv[2]);
        if (ioctl(fd, ASGN1_SET_MODE, &mode) == 0) {
            printf("[IOCTL-USER] Da chuyen sang Mode %d thanh cong!\n", mode);
        } else {
            perror("[IOCTL-USER] Loi goi ASGN1_SET_MODE");
        }
    } 
    else if (strcmp(argv[1], "version") == 0) {
        char ver[64];
        memset(ver, 0, sizeof(ver));
        if (ioctl(fd, ASGN1_GET_VERSION, ver) == 0) {
            printf("[IOCTL-USER] Phien ban Driver: %s\n", ver);
        } else {
            perror("[IOCTL-USER] Loi goi ASGN1_GET_VERSION");
        }
    } 
    else {
        printf("Lenh khong hop le: %s\n", argv[1]);
    }

    close(fd);
    return 0;
}
