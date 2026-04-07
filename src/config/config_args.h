#ifndef RK_CONFIG_ARGS_H
#define RK_CONFIG_ARGS_H

#include "config/config.h"

// Apply CLI argument overrides onto cfg.
// Recognises --watch, --db (bare filename), --log-level, --help / -h.
// --db rejects values containing '/'.
// Returns OK(NULL) on success, ERR() on unknown flag, missing value, or bad --db.
// OOM calls PANIC().
res_t rk_cfg_args_apply(rk_cfg_t *cfg, int argc, const char **argv);

#endif
