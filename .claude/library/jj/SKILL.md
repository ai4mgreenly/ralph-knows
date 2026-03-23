---
name: jj
description: Jujutsu (jj) version control workflow
---

# Jujutsu (jj)

## Work Directly on Main

**All work happens on `main`.** No feature branches, no topic branches, no PRs. Commit directly, push directly.

1. **Start fresh**: `jj git fetch` then `jj new main@origin`
2. **Do work**: Create commits as needed
3. **Push**: Move the `main` bookmark to HEAD and push it
4. **Done**: Return to step 1

## Starting Work

```bash
jj git fetch
jj new main@origin
```

This puts you on a fresh commit with `main@origin` as parent. All your work builds from here.

## Committing

When user says "commit", use `jj commit -m "msg"`:

```bash
jj commit -m "Add feature X"
```

Commits stack automatically. After 3 commits you have: `main@origin -> A -> B -> C (@)`

## Pushing to Main

When ready to push, move the local `main` bookmark to the top of your stack and push it:

```bash
# Move main bookmark to the latest committed revision (parent of @)
jj bookmark set main --to @-

# Push main (pushes ALL commits from main@origin to main)
jj git push --bookmark main
```

The working copy `@` itself is an empty in-progress commit — you want the bookmark on `@-`, which is your last real commit.

## Prohibited Operations

- Creating feature/topic bookmarks (work happens on `main` only)
- Opening PRs (direct push to `main`)
- Force pushing to `main`
- Pushing partial commit stacks

## Squashing (Permission Required)

**NEVER squash without explicit user permission.**

```bash
jj edit <revision>
jj squash -m "Combined message"
```

After squashing, update and push:
```bash
jj bookmark set main --to @-
jj git push --bookmark main
```

## Rebasing Remote Changes

Remote commits are **immutable** — `jj rebase` will fail on them. If `main@origin` has moved and you need to rebase your local stack onto it:

```bash
jj git fetch
jj rebase -d main@origin
```

**Never use `jj rebase` on remote commits themselves.**

## Recovery

All operations are logged:
```bash
jj op log
jj op restore <operation-id>
```

## Common Commands

| Task | Command |
|------|---------|
| Fetch remote | `jj git fetch` |
| Start fresh on main | `jj new main@origin` |
| Check status | `jj status` |
| View changes | `jj diff` |
| View log | `jj log` |
| Commit | `jj commit -m "msg"` |
| Move main bookmark to HEAD | `jj bookmark set main --to @-` |
| Push main | `jj git push --bookmark main` |

## Key Concepts

- **Working copy** (`@`): The in-progress commit being edited
- **`@-`**: The parent of the working copy — your last real commit
- **Bookmarks**: Named pointers to commits (like git branches); only `main` is used
- **main@origin**: The remote main branch
- **Commit stack**: Your commits from `main@origin` to `@-`, all pushed together
