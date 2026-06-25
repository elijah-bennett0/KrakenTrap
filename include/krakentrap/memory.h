#ifndef KRAKENTRAP_MEMORY_H
#define KRAKENTRAP_MEMORY_H

#include <sys/types.h>

int examine_memory(pid_t child_pid, unsigned long addr, int word_size);

#endif
