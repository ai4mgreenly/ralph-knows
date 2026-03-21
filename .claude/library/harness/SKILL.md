---
name: harness
description: Automated quality check loops with escalation and fix sub-agents
---

# Harness

Automated fix loops. Each harness runs a make target, spawns sub-agents to fix failures, commits on success, reverts on exhaustion.

**Pattern:** `.claude/harness/<name>/run` + `fix.prompt.md`

**Escalation:** sonnet:think → opus:think → opus:ultrathink

**History:** Each harness maintains `history.md` for cross-attempt learning. Truncated per-file, accumulates across escalation. Agents append summaries after each attempt so higher-level models can avoid repeating failed approaches.

## Quality Checks

The `check-*` namespace is reserved for quality checks. Only these scripts use the `check-` prefix. Each has a corresponding `fix-*` script that spawns sub-agents to fix failures.

### Core Quality Checks

The core quality checks are the default exit gate for all work. Run these in order when work is complete.

| Script | What it verifies |
|--------|------------------|
| `check-compile` | Code compiles cleanly |
| `check-link` | Linker succeeds |
| `check-filesize` | File size under 16KB |
| `check-unit` | Unit tests pass |
| `check-complexity` | Function complexity limits |

### Extended Quality Checks

Run only when explicitly requested.

| Script | What it verifies |
|--------|------------------|
| `check-sanitize` | Address/UB sanitizer clean |
| `check-tsan` | ThreadSanitizer clean |
| `check-valgrind` | Valgrind memcheck clean |
| `check-helgrind` | Valgrind helgrind clean |

## Always Use check-* Scripts, Never make Directly

**Never run `make check-*` targets directly.** Always use the `check-*` wrapper scripts instead. The harness scripts parse make's verbose output and return compact structured JSON (`{"ok": true/false, "items": [...]}`). Running make directly dumps raw compiler output into context, wasting thousands of tokens on noise. The harness scripts are drastically more token-efficient.

## CLI

- `.claude/bin/check-<name>` — symlinks to quality check harness run scripts
- `.claude/bin/fix-<name>` — symlinks to fix harness run scripts

All check scripts are on PATH via `.claude/bin/`.

## Running check-* Scripts

### Single file (development inner loop)

Use `--file=PATH` to scope a check to one file. This is fast (seconds) and is how you should work during active development.

```bash
check-compile --file=src/config/config.c
check-unit --file=tests/unit/config_test.c
```

After changing a file:
- `check-compile --file=PATH` after every edit
- `check-unit --file=PATH` when a test file exists
- Other checks as relevant to the change

Stay in this single-file loop. Do not run project-wide checks during active development.

### Project-wide (exit gate)

Run with no args to check everything. Use this as the exit gate when work is complete.

```bash
check-compile
check-link
check-filesize
check-unit
check-complexity
```

### Execution rules

- **Timeout:** Use 60 minute timeout (`timeout: 3600000`)
- **Foreground:** Always run in foreground (never use `run_in_background`)
- **Blocking:** No output until completion — do not tail or monitor, just wait
- **Output format:** Structured JSON: `{"ok": true/false, "items": [...]}`
