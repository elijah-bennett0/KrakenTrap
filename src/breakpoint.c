#include <stdio.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "krakentrap/breakpoint.h"

// *bp so that we can modify it rather than bp which just passes a copy
int enable_breakpoint(pid_t child_pid, breakpoint_t *bp) {
        unsigned long data_with_int3; // data after int3 instruction added

        // read the data
        errno = 0;
        // use -> because bp is a pointer
        bp->og_data = ptrace(PTRACE_PEEKTEXT, child_pid, (void *)bp->addr, 0);

        if (bp->og_data == (unsigned long)-1 && errno != 0) {
                perror("ptrace peektext");
                return -1;
        }

        /*
        0xff means only the lowest byte is 1s so we use ~0xff (~ = not) to flip it so that its all on>
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
                return -1;
        }

        bp->enabled = 1;
        bp->used = 1;
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

// take breakpoints and check if any are used. if theyre not, return it
breakpoint_t *find_free_breakpoint(breakpoint_t *breakpoints) {
        for (int i = 0; i < MAX_BREAKPOINTS; i++) {
                if (!breakpoints[i].used) {
                        return &breakpoints[i];
                }
        }
        return NULL;
}

// check the list of breakpoints for a specific address
breakpoint_t *find_breakpoint_by_addr(breakpoint_t *breakpoints, unsigned long addr) {
        for (int i = 0; i < MAX_BREAKPOINTS; i++) {
                if (breakpoints[i].used && breakpoints[i].addr == addr) {
                        return &breakpoints[i];
                }
        }
        return NULL;
}

void print_breakpoints(breakpoint_t *breakpoints) {

	for (int i = 0; i < MAX_BREAKPOINTS; i++) {
		if (breakpoints[i].used) {
			printf("Breakpoint set at: 0x%lx\n", breakpoints[i].addr);
		}
	}

}

int step_over_breakpoint(pid_t child_pid, breakpoint_t **pending_bp, int *wait_status) {
        if (*pending_bp == NULL) {
                return 0;
        }

        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
                perror("ptrace singlestep");
                return -1;
        }

        waitpid(child_pid, wait_status, 0);

        if (WIFEXITED(*wait_status)) {
                printf("child exited with code %d\n", WEXITSTATUS(*wait_status));
                *pending_bp = NULL;
                return 1;
        }

        if (WIFSIGNALED(*wait_status)) {
                printf("child killed by signal %d\n", WTERMSIG(*wait_status));
                *pending_bp = NULL;
                return 1;
        }

        if (enable_breakpoint(child_pid, *pending_bp) < 0) {
                printf("failed to re-enable breakpoint\n");
                return -1;
        }

        printf("breakpoint re-enabled at 0x%lx\n", (*pending_bp)->addr);
        *pending_bp = NULL;
        return 0;
}
