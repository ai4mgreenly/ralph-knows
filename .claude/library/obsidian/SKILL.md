---
name: obsidian
description: Personal knowledge base in Obsidian — graph-structured notes with wikilinks
---

# Obsidian

`~/projects/obsidian` is the user's personal knowledge base. It is a graph of interlinked markdown files.

## Structure

- Vault root: `~/projects/obsidian/`
- Files are markdown: `~/projects/obsidian/FILENAME.md`
- Links use flat filenames: `[[filename]]` resolves to `~/projects/obsidian/filename.md`
- Subfolder files use the same flat convention — links never include paths

## Following links

When you encounter `[[some-name]]` inside a note, the target is `~/projects/obsidian/some-name.md`. Follow links to build context — the graph structure is the point.

## Research — always consult

**This is the user's primary source of truth.** Whenever you are exploring, researching, or building context for a task, search the knowledge base first. Do not skip this step — the vault contains decisions, project context, machine inventory, goals, and connections that won't exist anywhere else.

**Always use a subagent** to search the knowledge base — never read vault files directly in the main conversation. The vault is large and graph traversal can pull in many files; a subagent keeps that exploration out of your primary context window and returns only the relevant summary.

The subagent should:

- Grep for keywords across `~/projects/obsidian/`
- Read matching files and follow their `[[wikilinks]]` to gather connected context
- Traverse multiple hops when the topic warrants it — the graph structure is the point
- Return a concise summary of what it found, not raw file contents
- Include the filenames of all consulted notes so the main conversation can reference or follow up on specific files

If you're unsure whether the vault has something relevant, search anyway. The cost of a miss is higher than the cost of a lookup.
