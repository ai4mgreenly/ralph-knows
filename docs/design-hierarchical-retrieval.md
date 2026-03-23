# Hierarchical Retrieval and Background Agents

Design document for ralph-knows's knowledge retrieval system. Primary consumers are AI agents. Human access via web interface is secondary.

## Design Principles

- Capability over cost
- Filesystem is source of truth; the database is a derived index
- AI agents are the primary consumers
- The system returns evidence, not conclusions

## Base Data Structure: Nodes

Everything is built on a single node structure. Nodes form a tree that mirrors the physical structure of content on disk.

### Node Types

| Type | Markdown maps to | Code maps to |
|------|-----------------|--------------|
| Directory | Filesystem folder | Filesystem folder |
| Document | A single file | A single file |
| Section | Heading block (H1–H3) and its content until the next heading at same or higher level | Function, class, struct, module, namespace (via tree-sitter) |
| Block | Paragraph, code fence, list, table, or other discrete element | Statements, comments, small logical units within a section |

The node types are universal. A parser per file type creates the nodes. Markdown parser splits on headings. Code parser splits on language constructs. Both produce the same tree shape. New file type support is just a new parser — nothing else changes.

### Node Record

The node itself is minimal:
- ID
- Type (directory, document, section, block)
- Parent node ID (forms the tree)
- File path
- Line range (start, end)
- Depth (for sections: heading level)
- Content hash (for change detection)

Everything else — summaries, embeddings, tags, edges — lives in separate tables with a foreign key back to the node ID. This means:
- A node can exist with zero metadata
- New metadata types require a new table, not schema changes to the nodes table
- Each background agent owns its own table and doesn't step on others

## Storage

PostgreSQL.

- **pgvector** for embeddings
- **Concurrent writers** — multiple background agents writing simultaneously
- **Full-text search** built in
- **LISTEN/NOTIFY** for agent coordination
- Recursive CTEs or Apache AGE for graph-style traversal

## Fanotify: The Entry Point

Fanotify watches the filesystem. When a file changes, ralph-knows parses it and creates or updates the node tree for that file. This is the first stage.

Fanotify doesn't know about downstream agents. It just writes nodes and sends a NOTIFY on its channel. The pipeline emerges from agent subscriptions.

## Background Agents

Each background agent:
- Owns its own table (summaries, tags, edges, etc.)
- Queries for nodes missing its metadata to find work
- Listens on NOTIFY channels for its dependencies
- Publishes to its own channel when work is done

No central scheduler. Each agent owns its own query strategy and priority ordering.

### Agent Pipeline

```
Fanotify writes nodes → NOTIFY new_nodes
  ├── Summarizer wakes → writes summaries → NOTIFY nodes_summarized
  ├── Classifier wakes → writes tags → NOTIFY nodes_classified
  │     └── Linker wakes (needs classification) → writes edges → NOTIFY nodes_linked
  └── Staleness Detector (periodic) → annotates nodes
        └── Consolidator (periodic) → merges fragmented knowledge
```

Each agent listens to the channels that represent its prerequisites. An agent can listen to multiple channels if it has multiple dependencies.

### Idle/Wake Behavior

Agents block on LISTEN when idle. Zero CPU when nothing's happening, instant response when work arrives. No polling, no backoff, no latency penalty after idle periods.

### Summarizer

Runs at every level except block (a paragraph is already short enough):

| Level | What the summary captures |
|-------|--------------------------|
| Section | "This section discusses X, decides Y, raises question Z." |
| Document | Synthesized from section summaries (not raw content). "This document is a design proposal for auth. Covers JWT migration, token refresh, proposes a timeline." |
| Directory | Synthesized from document summaries. "This folder contains 12 documents about auth: 3 proposals, 5 meeting notes, 2 specs, 2 decision records." |

Each level answers: "what's below me and why would you care?"

Document summaries are built from section summaries, not the full document. That's the point of the hierarchy — you don't re-read raw content at every level.

### Classifier

Labels individual nodes with metadata:
- **Type**: decision, spec, brainstorm, meeting notes, reference, tutorial, log
- **Status**: draft, active, superseded, archived
- **Domain/topic**: top-level subject areas

Runs on new content as it arrives. Classification feeds into the hierarchy — topic clusters are built from classifier output.

### Linker

Creates edges between nodes — the graph overlay on the tree:
- **Explicit links**: already in markdown, parsed and maintained
- **Implicit links**: documents discussing the same concepts without referencing each other

Relationship types: references, supersedes, contradicts, elaborates_on, discussed_in, etc.

Edges let agents traverse sideways across the hierarchy. "I found this decision — what led to it?" Follow the edges.

### Consolidator

Prevents entropy over time. Identifies clusters of related fragments across many files and produces canonical summaries. Runs periodically (weekly/monthly) on high-fragmentation areas.

### Staleness Detector

Marks content that's likely outdated based on age, superseded references, contradictions with newer content. Annotates nodes with staleness confidence for retrieval ranking. Doesn't delete anything.

## Search

### Hybrid Search on Summaries

A query runs FTS and vector search in parallel against the summaries table (all levels):
- **FTS** catches exact terms
- **Vector** catches semantic intent
- Results merged and scored

### Deduplication

Multiple summary levels for the same file may match a query. Deduplicate by grouping hits by file path and keeping the highest relevance score per file. The scoring mechanism determines which level wins — no hardcoded preference.

The result always resolves to a file path + line range regardless of which summary level matched.

### Response Shape

```json
{
  "answer": null,
  "results": [
    {
      "node_id": "a3f2c",
      "type": "section",
      "path": "projects/ralph-knows/docs/decisions/embedding-model.md",
      "lines": [42, 78],
      "heading": "## Decision",
      "summary": "Chose Gemini Embedding 2 for native multimodal support. Cost was secondary to capability. Local-first ruled out due to quality gap.",
      "relevance": 0.94,
      "tags": ["decision", "embeddings", "active"],
      "related": [
        {
          "node_id": "b7d1e",
          "relationship": "discussed_in",
          "path": "projects/ralph-knows/docs/meeting-notes/2026-03-22.md",
          "summary": "Conversation exploring all embedding approaches before settling on capability-first."
        }
      ]
    }
  ]
}
```

- **No `answer` field** — the system returns evidence, not conclusions. The consuming agent or web UI synthesizes the answer.
- **Summary included** — enough context to decide relevance without reading the source file.
- **Related nodes** — edges from the linker, enabling traversal to supporting context.
- **File path + line range** — the consumer reads the actual content from the filesystem.

## Open Questions

- How should directory-level hits be represented in results? Multiple files? A summary with pointers?
- Embedding model selection (Gemini Embedding 2 is a candidate — capability-first)
- How autonomous should background agents be vs. supervised?
- Hierarchies beyond the file tree: topic clusters, temporal views
- Scale strategy as data grows (~1TB/year)
- Web interface design for human consumers
- The retrieval planner / query decomposition layer
- Chat interface as API boundary for external agents
