#include "config/config_env.h"
#include "config/defaults.h"
#include <talloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>

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
        PANIC("Out of memory");
    }

    const char *db_env = getenv(RK_ENV_DB_PATH);
    if (db_env) {
        cfg->db_path = talloc_strdup(cfg, db_env);
    } else {
        cfg->db_path = talloc_asprintf(cfg, "%s%s", home, RK_DEFAULT_DB_PATH_SUFFIX);
    }
    if (!cfg->db_path) {
        PANIC("Out of memory");
    }

    const char *sock_env = getenv(RK_ENV_SOCKET_PATH);
    if (sock_env) {
        cfg->socket_path = talloc_strdup(cfg, sock_env);
    } else {
        const char *xdg_runtime = getenv("XDG_RUNTIME_DIR");
        if (xdg_runtime) {
            cfg->socket_path = talloc_asprintf(cfg, "%s%s", xdg_runtime,
                                               RK_DEFAULT_SOCKET_PATH_SUFFIX);
        } else {
            uid_t uid = getuid();
            cfg->socket_path = talloc_asprintf(cfg, "/run/user/%u%s", (unsigned)uid,
                                               RK_DEFAULT_SOCKET_PATH_SUFFIX);
        }
    }
    if (!cfg->socket_path) {
        PANIC("Out of memory");
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
