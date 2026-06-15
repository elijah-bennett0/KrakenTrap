# KrakenTrap Architecture

## Overview

KrakenTrap is a Linux x86-64 personal debugger and reverse-engineering workflow tool.

The core debugger will be written in C. A Python API will be added later so KrakenTrap can integrate with IntentLimit.

## High-Level Design

```text
IntentLimit
    |
    v
Python API / Plugin Layer
    |
    v
KrakenTrap C Core
    |
    +-- Process Control
    +-- Breakpoints
    +-- Registers
    +-- Memory
    +-- ELF Parsing
    +-- Disassembly
    +-- Session Logging
```

## Core Components

### Process Control

Handles launching and controlling target processes.

Responsibilities:

* Start a target binary
* Attach tracing with ptrace
* Continue execution
* Single-step instructions
* Handle process stops and signals
* Kill or detach from the target

### Breakpoint Manager

Handles software breakpoints.

Responsibilities:

* Save the original instruction byte
* Write the int3 breakpoint byte
* Detect breakpoint hits
* Restore the original byte
* Adjust RIP after a breakpoint
* Continue execution after a breakpoint

### Register Interface

Handles CPU register access.

Responsibilities:

* Read general-purpose registers
* Print register state
* Modify selected registers
* Track RIP, RSP, RBP, and other important registers

### Memory Interface

Handles target process memory.

Responsibilities:

* Read memory
* Write memory
* Dump stack memory
* Display memory in hex

### ELF Parser

Adds awareness of Linux ELF binaries.

Responsibilities:

* Read ELF headers
* Display entry point
* List sections
* Resolve simple symbols
* Support breakpoints by symbol name later

### Disassembly Engine

Displays instructions near the current instruction pointer.

Responsibilities:

* Disassemble around RIP
* Show instruction context
* Support a future context view

### Session Logger

Records analysis activity.

Responsibilities:

* Save command history
* Save breakpoint events
* Save crash events
* Save register snapshots
* Save memory snapshots
* Export reports later

## Planned Directory Structure

```text
include/krakentrap/
    process.h
    breakpoint.h
    registers.h
    memory.h
    elf.h
    disasm.h
    session.h
    repl.h

src/
    main.c
    process.c
    breakpoint.c
    registers.c
    memory.c
    elf.c
    disasm.c
    session.c
    repl.c

python/krakentrap/
    session.py
    timeline.py
    reports.py
    intentlimit_plugin.py
```

## Design Rules

* Build the C debugger first.
* Add Python integration only after the debugger core works.
* Keep the public version focused on authorized lab binaries.
* Prefer readable code over clever code.
* Make the tool useful for analysis, notes, snapshots, and reports.
