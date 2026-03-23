ralph-knows

Filesystem indexer for `~/projects`. Watches for changes via fanotify, maintains FTS5 and vector search indexes in SQLite, serves results over JSON-RPC on a Unix socket.

## Git

All work happens on `main`. Do not create branches. No feature branches, no topic branches, no temporary branches. Commit directly to `main`.

## Skills

> **This file is an index, not an encyclopedia.** Each skill listed below is a self-contained document you load on demand with `/load <name>`. If you need to understand something, load the relevant skill. Don't assume information is missing just because it isn't inlined here — check the skill table first, then load what you need. Resist the urge to front-load everything; load skills relevant to your current task and trust that the detail is there when you follow the pointer.

Use `/load <name>` to load a skill. Use `/load name1 name2` to load multiple.

### Goals

Use `/ralph-goal` to create, queue, and manage goals. When creating goals that involve C code, include a Skills section in the goal body so Ralph knows to load them:

```
## Skills
/load memory errors naming style
```

### Skillsets

Use `/skillset <name>` to load a skillset.
