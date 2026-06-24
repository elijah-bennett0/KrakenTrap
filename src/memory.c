#include <stdio.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>

int examine_memory(pid_t child_pid, unsigned long addr, int byte_count) {

	// print bytes
	unsigned long data;
	errno = 0;
	int word_size = sizeof(unsigned long);

	for (int offset = 0; offset < byte_count; offset += word_size) {
		data = ptrace(PTRACE_PEEKTEXT, child_pid, (void *)addr+offset, 0);


		if (data == (unsigned long)-1 && errno != 0) {
			perror("ptrace peektext");
			return -1;
		}

		int size = sizeof(data);

		printf("0x%lx: ", addr+offset);
		for (int i = 0; i < size; i++) {
			printf("%02lx ", (data >> (i * 8)) & 0xff);
		}

		// print ascii
		printf(" |");
		for (int i = 0; i < size; i++) {
			unsigned char byte = (data >> (i * 8)) & 0xff;
			if (byte >= 32 && byte <= 126) {
				printf("%c", byte);
			} else {
				printf(".");
			}
		}
		printf("|\n");
	}
	return 0;
}
