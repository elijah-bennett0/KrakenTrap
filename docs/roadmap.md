# KrakenTrap Roadmap

## v0.1 - Process Control

- Launch a target binary.
- Attach tracing with ptrace.
- Wait for initial stop.
- Continue execution.
- Exit cleanly.

## v0.2 - Registers

- Read general-purpose registers.
- Print register state.
- Single-step instructions.

## v0.3 - Breakpoints

- Set software breakpoint with int3.
- Restore original instruction byte.
- Adjust RIP after breakpoint hit.
- Continue after breakpoint.

## v0.4 - Memory

- Read target memory.
- Dump stack memory.
- Write target memory.

## v0.5 - ELF Awareness

- Parse ELF header.
- Show entry point.
- List sections.
- Resolve simple symbols.

## v0.6 - Disassembly

- Integrate Capstone.
- Disassemble around RIP.
- Add context view.

## v0.7 - Session Artifacts

- Save command history.
- Save snapshots.
- Add notes and tags.
- Export Markdown/JSON reports.

## v0.8 - Python API

- Wrap debugger functions.
- Add DebugSession class.
- Add IntentLimit plugin stub.
