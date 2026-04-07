#include "config/config_env.h"
#include "config/defaults.h"
#include <talloc.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "poison.h"

res_t rk_cfg_env_load(rk_cfg_t *cfg)
{
    const char *home = getenv("HOME");
    if (!home) {
        home = "/tmp";
    }

    const char *watch_env = getenv(RK_ENV_WATCH_PATH);
    if (watch_env) {
        cfg->watch_path = talloc_strdup(cfg, watch_env);
    } else {
        cfg->watch_path = talloc_asprintf(cfg, "%s%s", home, RK_DEFAULT_WATCH_PATH_SUFFIX);
    }
    if (!cfg->watch_path) {
        PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
    }

    const char *state_env = getenv(RK_ENV_STATE_DIR);
    if (state_env) {
        cfg->state_dir = talloc_strdup(cfg, state_env);
    } else {
        cfg->state_dir = talloc_asprintf(cfg, "%s%s", home, RK_DEFAULT_STATE_DIR_SUFFIX);
    }
    if (!cfg->state_dir) {
        PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
    }

    const char *log_env = getenv(RK_ENV_LOG_LEVEL);
    if (log_env) {
        if (strcasecmp(log_env, "debug") == 0) {
            cfg->log_level = RK_LOG_DEBUG;
        } else if (strcasecmp(log_env, "info") == 0) {
            cfg->log_level = RK_LOG_INFO;
        } else if (strcasecmp(log_env, "warn") == 0) {
            cfg->log_level = RK_LOG_WARN;
        } else if (strcasecmp(log_env, "error") == 0) {
            cfg->log_level = RK_LOG_ERROR;
        } else {
            return ERR(cfg, INVALID_ARG, "ralph-knows: invalid RALPH_KNOWS_LOG_LEVEL: %s", log_env);
        }
    } else {
        cfg->log_level = RK_DEFAULT_LOG_LEVEL;
    }

    return OK(NULL);
}
