# 001: Configuration Precedence

## Decision

Configuration values are resolved in this order (highest priority wins):

1. **Command-line arguments** (`--watch`, `--db`, `--socket`)
2. **Environment variables** (`FANDEX_WATCH_PATH`, `FANDEX_DB_PATH`, `FANDEX_SOCKET_PATH`)
3. **Compiled-in defaults** (`~/projects`, `~/.local/state/fandex/fandex.db`, `$XDG_RUNTIME_DIR/fandex/fandex.sock`)

There is no config file layer. The database will serve that role later,
slotting in between environment variables and compiled-in defaults.

## Rationale

This follows standard Linux convention: the more explicit and specific
the override, the higher priority it gets. Args are the most intentional
— typed for this specific invocation. Env vars are set per-session or
per-service. Defaults are always there as a fallback.
