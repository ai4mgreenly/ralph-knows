#ifndef FX_CONFIG_ENV_H
#define FX_CONFIG_ENV_H

#include "config/config.h"

// Populate cfg with compiled-in defaults, then apply env var overrides.
// Returns 0 on success, -1 on allocation failure.
int fx_config_env_load(fx_config_t *cfg);

#endif
