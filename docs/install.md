# Install

## Requirements

- Linux (kernel 5.1+)
- GCC
- talloc (`libtalloc-dev` on Debian/Ubuntu)
- `sudo` access (one-time, for granting fanotify capability)

## Build

```
make
```

This produces `bin/ralph-knows` (debug build). For a release build:

```
make BUILD=release
```

## Install

```
make install
```

This does three things:

1. Copies the binary to `~/.local/bin/ralph-knows`
2. Grants it `CAP_SYS_ADMIN` via `setcap` (requires sudo)
3. That's it — no root daemon, no setuid, no system-wide install

The capability lets ralph-knows use fanotify to watch `~/projects` as a
normal user. It's stored in filesystem extended attributes on the
binary and survives reboots.

## Verify

```
ralph-knows --help
```

If `~/.local/bin` isn't on your PATH, add it:

```
export PATH="$HOME/.local/bin:$PATH"
```

## What it watches

ralph-knows watches `~/projects` and indexes every file it finds, honoring
`.gitignore` at every level. Clone a repo into `~/projects` and it's
searchable within seconds.

## Where things live

| Path                                    | Purpose                               |
|-----------------------------------------|---------------------------------------|
| `~/.local/bin/ralph-knows`                   | Binary                                |
| `~/.local/state/ralph-knows/ralph-knows.db`       | SQLite database (the index)           |
| `$XDG_RUNTIME_DIR/ralph-knows/ralph-knows.sock`   | Unix socket (JSON-RPC API, see below) |

The database is a cache. Delete it and ralph-knows rebuilds the full index
from disk on next startup.

The socket path follows a fallback chain:
`$RALPH_KNOWS_SOCKET_PATH` → `$XDG_RUNTIME_DIR/ralph-knows/ralph-knows.sock` → `/tmp/ralph-knows-$UID/ralph-knows.sock`.

All paths are overridable via environment variables:
`RALPH_KNOWS_WATCH_PATH`, `RALPH_KNOWS_DB_PATH`, `RALPH_KNOWS_SOCKET_PATH`.

## After rebuilding

Every `make` clears the capability (it's an extended attribute on the
binary). Run `make install` again after each rebuild to restore it.
