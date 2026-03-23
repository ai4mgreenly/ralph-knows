#ifndef RK_CONFIG_H
#define RK_CONFIG_H

#include <stdbool.h>
#include <talloc.h>

#include "error.h"
#include "../log/log.h"

typedef struct rk_cfg {
    char *watch_path;
    char *db_path;
    char *socket_path;
    bool help;
    rk_log_level_t log_level;
} rk_cfg_t;

// Load config: env vars override compiled-in defaults; CLI args override env.
//   RALPH_KNOWS_WATCH_PATH  (default: ~/projects)
//   RALPH_KNOWS_DB_PATH     (default: ~/.local/state/ralph-knows/ralph-knows.db)
//   RALPH_KNOWS_SOCKET_PATH (default: /run/user/<uid>/ralph-knows/ralph-knows.sock)
//
// Allocates cfg as a child of ctx. On success returns OK(cfg).
// On unknown flag or missing value returns ERR(). OOM calls PANIC().
// Returns non-NULL cfg with cfg->help set if --help/-h was passed.
// Caller may call talloc_free(cfg) or let parent context handle cleanup.
res_t rk_cfg_load(TALLOC_CTX *ctx, int argc, const char **argv);

void rk_cfg_free(rk_cfg_t *cfg);

#endif
