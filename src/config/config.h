#ifndef RK_CONFIG_H
#define RK_CONFIG_H

#include <stdbool.h>
#include <talloc.h>

#include "error.h"
#include "../log/log.h"

typedef struct rk_cfg {
    char *watch_path;
    char *state_dir;     // ~/.local/state/ralph-knows or RALPH_KNOWS_STATE_DIR
    char *instance_name; // derived: basename(realpath(watch_path))
    char *db_name;       // instance_name or --db bare-filename override
    char *db_path;       // derived: state_dir/db_name.db
    char *socket_path;   // derived: XDG_RUNTIME_DIR/ralph-knows/db_name.sock
    bool help;
    bool version;
    rk_log_level_t log_level;
} rk_cfg_t;

// Load config: env vars override compiled-in defaults; CLI args override env.
//   RALPH_KNOWS_WATCH_PATH  (default: ~/projects)
//   RALPH_KNOWS_STATE_DIR   (default: ~/.local/state/ralph-knows)
//
// After env + args are applied, derives instance_name, db_path, and socket_path.
// Also creates state_dir and socket parent directory on disk.
//
// Allocates cfg as a child of ctx. On success returns OK(cfg).
// On unknown flag or missing value returns ERR(). OOM calls PANIC().
// Returns non-NULL cfg with cfg->help set if --help/-h was passed.
// Caller may call talloc_free(cfg) or let parent context handle cleanup.
res_t rk_cfg_load(TALLOC_CTX *ctx, int argc, const char **argv);

void rk_cfg_free(rk_cfg_t *cfg);

#endif
