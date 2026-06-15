# KrakenTrap Usage

## Overview

KrakenTrap is a Linux x86-64 personal debugger for authorized reverse engineering and lab debugging.

The first version will focus on process control, registers, breakpoints, memory inspection, and basic debugging commands.

## Build

```bash
make
```

## Build Example Binaries

```bash
./scripts/build_examples.sh
```

## Run KrakenTrap

```bash
./krakentrap ./examples/loop
```

## Basic Commands

### Run

```text
kt> run
```

Starts the target program under debugger control.

### Continue

```text
kt> continue
```

Continues the target process.

Alias:

```text
kt> c
```

### Single Step

```text
kt> step
```

Executes one instruction.

Alias:

```text
kt> si
```

### Registers

```text
kt> regs
```

Displays general-purpose registers.

Example:

```text
RIP  0x0000000000401136
RSP  0x00007fffffffdc20
RBP  0x00007fffffffdc40
RAX  0x0000000000000000
RBX  0x0000000000000000
```

### Breakpoint

```text
kt> break 0x401136
```

Sets a software breakpoint at an address.

Alias:

```text
kt> b 0x401136
```

### Delete Breakpoint

```text
kt> delete 0x401136
```

Removes a breakpoint.

### Memory Dump

```text
kt> x/32gx 0x7fffffffdc20
```

Dumps memory at an address.

Planned examples:

```text
kt> x/16gx $rsp
kt> x/32bx $rip
```

### Disassemble

```text
kt> disasm $rip
```

Displays instructions around RIP.

### Context

```text
kt> context
```

Planned context view:

* registers
* disassembly around RIP
* stack memory
* current signal
* stop reason

### Quit

```text
kt> quit
```

Exits KrakenTrap.

Alias:

```text
kt> q
```

## Example Session

```text
$ ./krakentrap ./examples/loop

KrakenTrap v0.1
Target: ./examples/loop

kt> run
target stopped

kt> regs
RIP  0x00007ffff7fe3290
RSP  0x00007fffffffdc20

kt> break 0x401136
breakpoint set at 0x401136

kt> continue
hit breakpoint at 0x401136

kt> step
single step complete

kt> quit
```

## Example Binaries

The examples directory will contain safe local test programs.

Planned examples:

```text
examples/loop.c
examples/crash.c
examples/args.c
examples/vuln1.c
```

## Session Output

Future versions will save session data under:

```text
sessions/
```

Planned structure:

```text
sessions/
    SESSION_ID/
        commands.log
        timeline.json
        report.md
        snapshots/
```
