#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DBFS_FILE_PATH  "/sys/kernel/debug/paddr/output"

struct packet {
	pid_t pid;
	unsigned long vaddr;
	unsigned long paddr;
};

int main(void) {

	int fd, ret;
	struct packet pckt;

	pckt.pid = getpid();
	pckt.vaddr = (unsigned long)malloc(sizeof(unsigned long));
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

	printf("pid: %u  vaddr: %lx  paddr: %lx\n", pckt.pid, pckt.vaddr, pckt.paddr);

	close(fd);
	free((void*)(pckt.vaddr));

	return 0;
}

