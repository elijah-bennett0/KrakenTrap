#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "krakentrap/repl.h"
#include "krakentrap/breakpoint.h"
#include "krakentrap/registers.h"
#include "krakentrap/memory.h"
#include "krakentrap/color.h"

void run_debugger(pid_t child_pid) {

        int wait_status;
        char command[128];

        waitpid(child_pid, &wait_status, 0);

        breakpoint_t breakpoints[MAX_BREAKPOINTS];
        breakpoint_t *pending_bp = NULL;

        for (int i = 0; i < MAX_BREAKPOINTS; i++) {
                breakpoints[i].addr = 0;
                breakpoints[i].og_data = 0;
                breakpoints[i].enabled = 0;
                breakpoints[i].used = 0;
        }

        if (!WIFSTOPPED(wait_status)) {
                printf(KT_WARN "Child didnt stop as expected\n");
                return;
        }

        while (1) {

                fputs("KrakenTrap > ", stdout);
                fflush(stdout);

                if (fgets(command, sizeof(command), stdin) == NULL) {
                        break;
                }

		if (command[0] == '\n') {
			continue;
		}

                if (command[0] == 'q') {
                        kill(child_pid, SIGKILL);
                        waitpid(child_pid, &wait_status, 0);
                        break;
                }

                if (command[0] == 'p') {
			//char *endptr = NULL;
			if (command[2] == 'r') {
                        	print_registers(child_pid);
				continue;
			}

			if (command[2] == 'b') {
				print_breakpoints(breakpoints);
				continue;
			}

			printf(KT_INFO "Usage: p <(r)egisters , (b)reakpoints>\n");
			continue;
                }

                if (command[0] == 'b') {
                        char *endptr = NULL;
                        unsigned long addr = strtoul(command + 1, &endptr, 0);

                        if (endptr == command + 1) {
                                printf(KT_INFO "Usage: b <address>\n");
                                continue;
                        }

                        if (find_breakpoint_by_addr(breakpoints, addr) != NULL) {
                                printf(KT_INFO "Breakpoint already set at 0x%lx\n", addr);
                                continue;
                        }

                        breakpoint_t *bp = find_free_breakpoint(breakpoints);

                        if (bp == NULL) {
                                printf(KT_INFO "Max breakpoints reached\n");
                                continue;
                        }

                        bp->addr = addr;
                        bp->og_data = 0;
                        bp->enabled = 0;
                        bp->used = 0;

                        if (enable_breakpoint(child_pid, bp) < 0) {
                                printf(KT_ERR "Failed to set breakpoint\n");
                                continue;
                        }

                        printf(KT_GOOD "Breakpoint set at 0x%lx\n", bp->addr);
                        continue;
                }

                if (command[0] == 'c') {

                        int step_result = step_over_breakpoint(child_pid, &pending_bp, &wait_status);

                        // error
                        if (step_result < 0) {
                                return;
                        }

                        // child killed/exited
                        if (step_result > 0) {
                                break;
                        }

                        // ptrace continue until hit breakpoint
                        if (ptrace(PTRACE_CONT, child_pid, 0, 0) < 0) {
				printf(KT_ERR "Ptrace error\n");
                                perror("ptrace cont");
                                return;
                        }

                        waitpid(child_pid, &wait_status, 0);

                        if (WIFEXITED(wait_status)) {
                                printf(KT_WARN "Child exited with code %d\n", WEXITSTATUS(wait_status));
                                break;
                        }

                        if (WIFSIGNALED(wait_status)) {
                                printf(KT_WARN "Child killed by signal %d\n", WTERMSIG(wait_status));
                                break;
                        }

                        if (WIFSTOPPED(wait_status)) {

                                printf(KT_WARN "Child stopped with signal %d\n", WSTOPSIG(wait_status));

                                if (WSTOPSIG(wait_status) == SIGTRAP) {
                                        unsigned long rip = get_instruction_pointer(child_pid);
                                        unsigned long hit_addr = rip - 1;

                                        breakpoint_t *hit_bp = find_breakpoint_by_addr(breakpoints, hit_addr);

                                        if (hit_bp != NULL && hit_bp->enabled) {
                                                printf(KT_INFO "Hit breakpoint at 0x%lx\n", hit_bp->addr);
                                                print_registers(child_pid);

                                                if (disable_breakpoint(child_pid, hit_bp) < 0) {
                                                        return;
                                                }

                                                set_instruction_pointer(child_pid, hit_bp->addr);
                                                pending_bp = hit_bp;

                                                printf(KT_INFO "Breakpoint restored to original instruction\n");
                                        }

                                }
                        }
                        continue;
                }

                if (command[0] == 's') {
                        int step_result = step_over_breakpoint(child_pid, &pending_bp, &wait_status);

                        if (step_result < 0) {
                                return;
                        }

                        if (step_result > 0) {
                                break;
                        }

                        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
				printf(KT_ERR "Ptrace error\n");
                                perror("ptrace singlestep");
                                return;
                        }

                        waitpid(child_pid, &wait_status, 0);

                        if (WIFEXITED(wait_status)) {
                                printf(KT_WARN "Child exited with status %d\n", WEXITSTATUS(wait_status));
                                break;
                        }

                        if (WIFSTOPPED(wait_status)) {
                                printf(KT_INFO "Single step complete\n");
                                print_registers(child_pid);
                        }

                        continue;
                }

                if (command[0] == 'h') {
                        printf(KT_INFO "Commands: (b)reak <addr> , (p)rint [registers] , (s)tep , (c)ontinue , (q)uit , e(x)amine <address> <byte_count> , (h)elp\n");
                        continue;
                }

		if (command[0] == 'x') {
			char *endptr = NULL;
			unsigned long addr = strtoul(command + 1, &endptr, 0);

			int byte_count = 8; // default
			if (*endptr != '\n' && *endptr != '\0') {
				byte_count = strtol(endptr, &endptr, 0);
			}

			if (examine_memory(child_pid, addr, byte_count) < 0) {
				printf(KT_ERR "Failed to examine memory\n");
			}
			continue;
		}

                printf(KT_ERR "Unknown command\n");
                printf(KT_INFO "Commands: (b)reak <addr> , (p)rint [registers] , (s)tep , (c)ontinue , (q)uit , e(x)amine <address> <byte_count> , (h)elp\n");
        }
}
