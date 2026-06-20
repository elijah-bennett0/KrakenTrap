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
#include <unistd.h>
#include <sys/types.h>

#include "krakentrap/process.h"
#include "krakentrap/repl.h"

int main(int argc, char** argv) {

	pid_t child_pid;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <target_binary>\n", argv[0]);
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

