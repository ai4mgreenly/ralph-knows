# 2026-04-05 13:16 — finished the rename, fixed a fanotify bug

## What

Completed the `fandex` → `ralph-knows` rename that started earlier today. Every user-visible reference now matches the project's actual name: binary (`bin/ralph-knows`), install path, env vars (`RALPH_KNOWS_*`), state paths (`~/.local/state/ralph-knows/`), log messages, error prefixes, docs, skills. The internal `fx_` symbol prefix is also gone — renamed wholesale to `rk_` across 27 files. Only `devlog/` entries still mention the old names, because they are historical records of the decisions themselves.

Separately, fixed a latent `open_by_handle_at` bug in the watch module that surfaced the moment we pointed it at a different filesystem from the cwd.

## Why these choices

**Finish the rename in one push.** The earlier "leave `fx_` for later" note was defensible in isolation, but once we were actively touching every file in the project anyway, the churn cost was already paid. Splitting it across sessions would mean two rounds of build breakage, two rounds of review, and a codebase that looks half-renamed to anyone joining later. Do it once, do it completely.

**`open_by_handle_at` needs a real mount fd, not `AT_FDCWD`.** The watch module was passing `AT_FDCWD` as the mount file descriptor — which silently "worked" only because the cwd happened to be on the same filesystem as the events being watched. Standing up the dev loop-mount (`/mnt/projects-dev`) exposed this immediately: running the binary from `~/projects/ralph-knows` (root ext4) while watching a different filesystem produced `ESTALE` on every event. Fix is to keep the watch-path dirfd open across `rk_watch_init` and store it on the struct, then pass it to `open_by_handle_at`. This is the kind of bug that only appears once you have multi-filesystem coverage — good argument for the dev sandbox pulling its weight already.

## Carrying forward

- `docs/service.md` is still a one-line stub. Systemd user unit(s) for prod and dev are the next obvious piece of infrastructure now that the dev sandbox is live and the binary name is stable.
- The `RALPH_KNOWS_*` env vars are set in `.envrc`, but there's still no explicit "dev mode" selection mechanism — direnv happens to point dev at the right paths because we're working inside the checkout. A real dev/prod split will need a cleaner story once the systemd units exist.
