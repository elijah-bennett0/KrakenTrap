#ifndef KRAKENTRAP_REGISTERS_H
#define KRAKENTRAP_REGISTERS_H

#include <sys/types.h>

void print_registers(pid_t child_pid);
unsigned long get_instruction_pointer(pid_t child_pid);
void set_instruction_pointer(pid_t child_pid, unsigned long addr);

#endif
