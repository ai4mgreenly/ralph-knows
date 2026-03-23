#ifndef RK_CONFIG_ENV_H
#define RK_CONFIG_ENV_H

#include "config/config.h"

// Populate cfg with compiled-in defaults, then apply env var overrides.
// OOM calls PANIC(). Returns OK(NULL) on success, ERR() on failure.
res_t rk_cfg_env_load(rk_cfg_t *cfg);

#endif
