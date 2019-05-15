#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>

#define DBFS_FILE_PATH  "/sys/kernel/debug/paddr/output"
#define PADDR   0x42341000
//define PADDR 0x20000

struct packet {
        pid_t pid;
        unsigned long vaddr;
        unsigned long paddr;
};

int main(void)
{
        int fd, mem, ret;
        struct packet pckt;

        mem = open("/dev/mem", O_RDWR);

        pckt.pid = getpid();
        pckt.vaddr = (unsigned long)mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED, mem, PADDR);
        pckt.paddr = 0;

        fd = open(DBFS_FILE_PATH, O_RDWR);

        if (fd < 0) {
                printf("debugfs file open error\n");
                exit(-1);
        }

        ret = read(fd, &pckt, sizeof(struct packet));

        if (ret < 0) {
                printf("debugfs input file write failed\n");
                exit(-1);
        }

        assert(pckt.paddr == PADDR);
        printf("[TEST CASE]    PASS\n");

        close(fd);
        munmap((void*)pckt.vaddr, sizeof(unsigned long));

        return 0;
}
