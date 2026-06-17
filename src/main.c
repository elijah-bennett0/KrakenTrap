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
	unsigned icounter = 0;
	printf("debugger started\n");

	// waits for child to stop on first instruction
	wait(&wait_status);

	while (WIFSTOPPED(wait_status)) {
		icounter++;

		// print registers every 10000 instructions for now
		if (icounter % 10000 == 0) {
			print_registers(child_pid);
		}

		// execute another instruction
		if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
			perror("ptrace");
			return;
		}
		// wait for child to stop on next instruction
		wait(&wait_status);
	}

	printf("the child executed %u instructions\n", icounter);
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

