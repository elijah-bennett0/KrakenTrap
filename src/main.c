/*
 * KrakenTrap - initial ptrace prototype
 *
 * This early prototype is based on Eli Bendersky's
 * "How debuggers work: Part 1 - Basics".
 *
 * Original article:
 * https://eli.thegreenplace.net/2011/01/23/how-debuggers-work-part-1
 *
 * Original code samples:
 * https://github.com/eliben/code-for-blog
 *
 * Eli's code-for-blog repository states that, unless otherwise noted,
 * the code samples are public domain / Unlicense.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>

typedef struct {
	unsigned long addr;
	unsigned long og_data;
	int enabled;
} breakpoint_t;

void set_instruction_pointer(pid_t child_pid, unsigned long addr) {

	struct user_regs_struct regs;

	if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) < 0) {
		perror("ptrace getregs");
		return;
	}

	regs.rip = addr;

	if (ptrace(PTRACE_SETREGS, child_pid, 0, &regs) < 0) {
		perror("ptrace setregs");
		return;
	}
}

// *bp so that we can modify it rather than bp which just passes a copy
int enable_breakpoint(pid_t child_pid, breakpoint_t *bp) {
	unsigned long data_with_int3; // data after int3 instruction added

	// read the data
	errno = 0;
	// use -> because bp is a pointer
	bp->og_data = ptrace(PTRACE_PEEKTEXT, child_pid, (void *)bp->addr, 0);

	if (bp->og_data == (unsigned long)-1 && errno != 0) {
		perror("ptrace peektext");
		return 0;
	}

	/*
	0xff means only the lowest byte is 1s so we use ~0xff (~ = not) to flip it so that its all ones except the lowest byte
	then we and it (&) with data so that we can set the lowest byte of the original data to 0
	then we OR (|) it with 0xcc so that the last byte becomes 0xcc

	example:
	             data  = 0xabababab
	             ~0xff = 0xffffff00
	0xabababab & ~0xff = 0xababab00
	0xababab00 | 0xcc  = 0xabababcc
	*/

	data_with_int3 = (bp->og_data & ~0xff) | 0xcc;
	// write the data with int3
	if (ptrace(PTRACE_POKETEXT, child_pid, (void *)bp->addr, (void *)data_with_int3) < 0) {
		perror("ptrace poketext");
		return 0;
	}

	bp->enabled = 1;
	return 0;
}

int disable_breakpoint(pid_t child_pid, breakpoint_t *bp) {

	if (!bp->enabled) {
		return 0;
	}

	if (ptrace(PTRACE_POKETEXT, child_pid, (void *)bp->addr, (void *)bp->og_data) < 0) {
		perror("ptrace restore breakpoint");
		return -1;
	}

	bp->enabled = 0;
	return 0;
}

void print_registers(pid_t child_pid) {

	// ptrace(PTRACE_GETREGS/GETFPREGS, pid, 0, &struct);
	struct user_regs_struct regs; // from sys/user.h

	if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) < 0) {
		perror("ptrace getregs");
		return;
	}

	// not a real clean way to loop through the registers so we'll just use a manual table

	printf("=-=-=-= Register Table =-=-=-=\n");
	printf("RIP: 0x%llx\n", regs.rip);
	printf("RSP: 0x%llx\n", regs.rsp);
	printf("RBP: 0x%llx\n", regs.rbp);
	printf("RAX: 0x%llx\n", regs.rax);
	printf("RBX: 0x%llx\n", regs.rbx);
	printf("RDX: 0x%llx\n", regs.rdx);
	printf("RSI: 0x%llx\n", regs.rsi);
	printf("RDI: 0x%llx\n", regs.rdi);
	printf("RCX: 0x%llx\n", regs.rcx);
	printf("R8 : 0x%llx\n", regs.r8);
	printf("R9 : 0x%llx\n", regs.r9);
	printf("R10: 0x%llx\n", regs.r10);
	printf("R11: 0x%llx\n", regs.r11);
	printf("R12: 0x%llx\n", regs.r12);
	printf("R13: 0x%llx\n", regs.r13);
	printf("R14: 0x%llx\n", regs.r14);
	printf("R15: 0x%llx\n", regs.r15);
	printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");

}

void run_target(const char* progname) {

	printf("target started. running '%s'\n", progname);

	// from sys/ptrace.h : long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
	// request is a PTRACE_ constant, then pid, then addr/data pointer for mem manipulation

	if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
		perror("ptrace");
		return;
	}

	execl(progname, progname, (char *)NULL);
}

void run_debugger(pid_t child_pid, unsigned long breakpoint_addr) {

	int wait_status;
	printf("debugger started\n");

	waitpid(child_pid, &wait_status, 0);

	if (!WIFSTOPPED(wait_status)) {
		printf("child did not stop as expected\n");
		return;
	}
	//unsigned long og_data = set_breakpoint(child_pid, breakpoint_addr);

	breakpoint_t bp;
	bp.addr = breakpoint_addr;
	bp.og_data = 0;
	bp.enabled = 0;

	if (enable_breakpoint(child_pid, &bp) < 0) {
		printf("failed to set breakpoint\n");
		return;
	}

	printf("breakpoint set at 0x%lx\n", breakpoint_addr);

	// ptrace continue until hit breakpoint
	if (ptrace(PTRACE_CONT, child_pid, 0, 0) < 0) {
		perror("ptrace cont");
		return;
	}

	waitpid(child_pid, &wait_status, 0);

	if (WIFSTOPPED(wait_status)) {
		printf("child stopped with signal %d\n", WSTOPSIG(wait_status));
		printf("hit breakpoint at 0x%lx\n", breakpoint_addr);
		print_registers(child_pid);
	}

	if (disable_breakpoint(child_pid, &bp) < 0) {
		return;
	}

	printf("restored original instruction at 0x%lx\n", bp.addr);

	set_instruction_pointer(child_pid, bp.addr); // this is to rewind the instruction pointer back to the main address

	if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
		perror("ptrace singlestep");
		return;
	}

	waitpid(child_pid, &wait_status, 0);
	printf("single stepped original instruction\n");

	if (ptrace(PTRACE_CONT, child_pid, 0, 0) < 0) {
		perror("ptrace cont");
		return;
	}

	while (1) {

		waitpid(child_pid, &wait_status, 0);
		// exit
		if (WIFEXITED(wait_status)) {
			printf("child exited with code %d\n", WEXITSTATUS(wait_status));
			break;
		}
		// killed
		if (WIFSIGNALED(wait_status)) {
			printf("child killed by signal %d\n", WTERMSIG(wait_status));
			break;
		}
		// stopped
		if (WIFSTOPPED(wait_status)) {
			printf("child stopped with signal %d\n", WSTOPSIG(wait_status));
			break;
		}
	}
}

int main(int argc, char** argv) {

	pid_t child_pid;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <target_binary> <breakpoint_address>\n", argv[0]);
		return -1;
	}

	char *endptr = NULL;
	unsigned long breakpoint_addr = strtoul(argv[2], &endptr, 0);

	if (endptr == argv[2] || *endptr != '\0') {
		fprintf(stderr, "invalid breakpoint address %s\n", argv[2]);
		return -1;
	}

	child_pid = fork();
	if (child_pid == 0) {
		run_target(argv[1]);
	} else if (child_pid > 0) {
		run_debugger(child_pid, breakpoint_addr);
	} else {
		perror("fork");
		return -1;
	}

	return 0;
}

