#ifndef FX_CONFIG_ARGS_H
#define FX_CONFIG_ARGS_H

#include "config/config.h"

// Apply CLI argument overrides onto cfg.
// Recognises --watch, --db, --socket, --help / -h.
// Returns 0 on success, nonzero on unknown flag or missing value.
int fx_config_args_apply(fx_config_t *cfg, int argc, const char **argv);

#endif
