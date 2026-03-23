# 2026-04-05 13:03 — dev sandbox, watch filters, project rename

## What

Trimmed the fanotify watcher's signal down to what actually matters for indexing, stood up a dev filesystem isolated from prod, and renamed the project from `fandex` to `ralph-knows`.

## Why these choices

**Filter out VCS internals and whitelist source extensions.** The raw fanotify stream on `/mnt/projects` is dominated by jj's git import/export locks and Vim's atomic-save dance (the `4913` probe file, `~` backups, rename-over). None of that belongs in an index. We added two stacked filters in `process_fan_event`: drop anything under `/.jj/` or `/.git/`, and drop anything whose extension isn't on a whitelist (`.md`, `.txt`, `.c`, `.h` for now). The whitelist is deliberately small — it's easier to add extensions when we hit a concrete need than to debug noise from an overly permissive default. Both filters are post-resolution (after we've already reconstructed the full path from the event's file handle), which keeps them trivial to extend later.

**Loop-mounted ext4 image for the dev sandbox, not a dedicated drive.** The goal is to run an early prod instance of ralph-knows on `/mnt/projects` while doing active development without cross-contamination. fanotify's `FAN_MARK_FILESYSTEM` watches an entire filesystem, so dev and prod need to live on separate filesystems — not just separate directories. Options considered: dedicate the free `nvme2n1` drive (rejected as overkill for a dev sandbox and a waste of hardware we might want for something else), or create a loop-mounted image file. We went with a 5GB sparse ext4 image at `~/.local/state/ralph-knows/projects-dev.img` mounted at `/mnt/projects-dev` via fstab. Sparse, so it only consumes what it actually uses; in fstab with `nofail` so a missing image never blocks boot; user-owned because this sandbox is single-user (`ai4mgreenly` only) — no shared-group setup like prod. The loop file gives us a first-class filesystem from the kernel's perspective, so fanotify treats it identically to the real drive.

**Rename fandex → ralph-knows.** The project is joining the Ralph ecosystem and the name should reflect that. The local checkout folder was already `ralph-knows`; this session brought GitHub, the bare repo at `/mnt/store/git/ai4mgreenly/`, and both local clones' `origin` URLs into alignment. GitHub's automatic redirect handles any stale links. The source tree still uses the `fx_` prefix and `fandex` binary name internally — those are separate concerns and will be addressed when they become friction, not preemptively.

## Deferred

- Second binary / second systemd user unit for the dev instance. Config already namespaces dev state (`fandex-dev` db path, socket path) but we haven't actually wired up how "dev mode" gets selected, nor written either systemd unit file. `docs/service.md` is still a one-line stub.
- Renaming the `fx_` symbol prefix and binary name. No reason to churn working code yet.
- Gitignore-aware filtering in the watch module (beyond the hardcoded `.jj`/`.git` drop). That's the real long-term solution; the current filter is a floor, not a ceiling.
