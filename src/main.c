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

unsigned long set_breakpoint(pid_t child_pid, unsigned long addr) {

	unsigned long data; // og data
	unsigned long data_with_int3; // data after int3 instruction added

	// read the data
	data = ptrace(PTRACE_PEEKTEXT, child_pid, (void *)addr, 0);
	if (data == (unsigned long)-1) {
		perror("ptrace peektext");
		return 0;
	}

	data_with_int3 = (data & ~0xff) | 0xcc;
	// write the data with int3
	if (ptrace(PTRACE_POKETEXT, child_pid, (void *)addr, (void *)data_with_int3) < 0) {
		perror("ptrace poketext");
		return 0;
	}
	return data;
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

void run_debugger(pid_t child_pid) {

	int wait_status;
	printf("debugger started\n");

	waitpid(child_pid, &wait_status, 0);

	if (!WIFSTOPPED(wait_status)) {
		printf("child did not stop as expected\n");
		return;
	}

	unsigned long main_addr = 0x401156; // hardcode main address for now
	unsigned long og_data = set_breakpoint(child_pid, main_addr);

	if (og_data == 0) {
		printf("failed to set breakpoint\n");
		return;
	}

	printf("breakpoint set at 0x%lx\n", main_addr);

	// ptrace continue until hit breakpoint
	if (ptrace(PTRACE_CONT, child_pid, 0, 0) < 0) {
		perror("ptrace cont");
		return;
	}

	waitpid(child_pid, &wait_status, 0);

	if (WIFSTOPPED(wait_status)) {
		printf("child stopped with signal %d\n", WSTOPSIG(wait_status));
		printf("hit breakpoint at 0x%lx\n", main_addr);
		print_registers(child_pid);
	}

	if (ptrace(PTRACE_POKETEXT, child_pid, (void *)main_addr, (void *)og_data) < 0) {
		perror("ptrace restore");
		return;
	}

	printf("restored original instruction at 0x%lx\n", main_addr);
}

int main(int argc, char** argv) {

	pid_t child_pid;

	if (argc < 2) {
		fprintf(stderr, "Expected a program name as an argument\n");
		return -1;
	}

	child_pid = fork();
	if (child_pid == 0) {
		run_target(argv[1]);
	} else if (child_pid > 0) {
		run_debugger(child_pid);
	} else {
		perror("fork");
		return -1;
	}

	return 0;
}

