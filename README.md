# ralph-knows

Filesystem indexer for `~/projects`. Watches for changes via fanotify, maintains FTS5 and vector search indexes in SQLite, serves results over JSON-RPC on a Unix socket.
