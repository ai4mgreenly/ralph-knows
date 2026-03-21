# fandex

Search index for `~/projects`. Watches the filesystem for changes, keeps full-text and semantic search indexes up to date, serves results over a Unix socket.

## How it works

Files on disk are the source of truth. fandex monitors `~/projects` via fanotify and automatically indexes every file it sees. Edit a file, fandex re-indexes it. Delete a file, fandex drops it from the index. The database is a cache — delete it and fandex rebuilds from the current filesystem.

`.gitignore` files control what gets indexed. If git ignores it, fandex ignores it.

## Guides

- [Install](install.md) — build, install, grant capabilities
- [Service](service.md) — run as a systemd user service
- [Searching](searching.md) — query the index via JSON-RPC

## Decisions

- [001: Config precedence](adr/001-config-precedence.md) — args > env vars > defaults

## Reference

- [Design: fanotify watcher](design-fanotify-watcher.md) — architecture and design decisions
