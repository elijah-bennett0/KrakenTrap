#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>

#include "krakentrap/process.h"
#include "krakentrap/color.h"

void run_target(const char* progname) {

        printf(KT_GOOD "Target started. running '%s'\n", progname);

        // from sys/ptrace.h : long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void>
        // request is a PTRACE_ constant, then pid, then addr/data pointer for mem manipulation

        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
		printf(KT_ERR "Ptrace error\n");
                perror("ptrace");
                return;
        }

        execl(progname, progname, (char *)NULL);
}
