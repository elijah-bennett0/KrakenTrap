#ifndef KRAKENTRAP_BREAKPOINT_H
#define KRAKENTRAP_BREAKPOINT_H

#include <sys/types.h>

#define MAX_BREAKPOINTS 32

typedef struct {
        unsigned long addr;
        unsigned long og_data;
        int enabled;
        int used;
} breakpoint_t;

int enable_breakpoint(pid_t child_pid, breakpoint_t *bp);
int disable_breakpoint(pid_t child_pid, breakpoint_t *bp);

breakpoint_t *find_free_breakpoint(breakpoint_t *breakpoints);
breakpoint_t *find_breakpoint_by_addr(breakpoint_t *breakpoints, unsigned long addr);
void print_breakpoints(breakpoint_t *breakpoints);
int step_over_breakpoint(pid_t child_pid, breakpoint_t **pending_bp, int *wait_status);

#endif
