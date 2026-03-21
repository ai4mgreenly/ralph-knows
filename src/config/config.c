#include "config/config.h"
#include "config/config_env.h"
#include "config/config_args.h"

#include <stdlib.h>

#include "poison.h"

fx_config_t *fx_config_load(int argc, const char **argv)
{
    fx_config_t *cfg = calloc(1, sizeof(*cfg));
    if (!cfg) {
        return NULL;
    }

    if (fx_config_env_load(cfg) != 0) {
        fx_config_free(cfg);
        return NULL;
    }

    if (fx_config_args_apply(cfg, argc, argv) != 0) {
        fx_config_free(cfg);
        return NULL;
    }

    return cfg;
}

void fx_config_free(fx_config_t *cfg)
{
    if (!cfg) {
        return;
    }

    free(cfg->watch_path);
    free(cfg->db_path);
    free(cfg->socket_path);
    free(cfg);
}
