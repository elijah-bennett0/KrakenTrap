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

void run_target(const char* progname) {

	printf("target started. running '%s'\n", progname);

	// from sys/ptrace.h : long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
	// request is a PTRACE_ constant, then pid, then addr/data pointer for mem manipulation

	if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
		perror("ptrace");
		return;
	}

	execl(progname, progname, 0);
}

void run_debugger(pid_t child_pid) {

	int wait_status;
	unsigned icounter = 0;
	printf("debugger started\n");

	// waits for child to stop on first instruction
	wait(&wait_status);

	while (WIFSTOPPED(wait_status)) {
		icounter++;

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

