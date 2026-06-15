# KrakenTrap Goals

## Purpose

KrakenTrap is a Linux x86-64 personal debugger and reverse-engineering workflow tool. It is designed to help capture debugging sessions as structured analysis artifacts.

## Primary Goals

- Build a working debugger core in C.
- Support basic process control with ptrace.
- Implement software breakpoints.
- Inspect registers and memory.
- Display disassembly around RIP.
- Save session timelines, notes, and snapshots.
- Expose a Python API for future IntentLimit integration.

## Short-Term Goals

- Launch a target binary.
- Continue execution.
- Single-step.
- Read registers.
- Set and hit a breakpoint.
- Inspect memory.
- Quit cleanly.

## Long-Term Goals

- ELF symbol resolution.
- Capstone disassembly.
- Session timeline.
- Snapshot diffing.
- Markdown/JSON reports.
- Python scripting hooks.
- IntentLimit plugin.
