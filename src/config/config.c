#include "config/config.h"
#include "config/config_env.h"
#include "config/config_args.h"
#include <talloc.h>

#include "poison.h"

res_t rk_cfg_load(TALLOC_CTX *ctx, int argc, const char **argv)
{
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    if (!cfg) {
        PANIC("Out of memory");
    }

    res_t r = rk_cfg_env_load(cfg);
    if (is_err(&r)) {
        talloc_free(cfg);
        return r;
    }

    r = rk_cfg_args_apply(cfg, argc, argv);
    if (is_err(&r)) {
        talloc_free(cfg);
        return r;
    }

    return OK(cfg);
}

void rk_cfg_free(rk_cfg_t *cfg)
{
    talloc_free(cfg);
}
