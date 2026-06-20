#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include "krakentrap/registers.h"

unsigned long get_instruction_pointer(pid_t child_pid) {
        struct user_regs_struct regs;

        if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) < 0) {
                perror("ptrace getregs");
                return 0;
        }
        return regs.rip;
}

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
