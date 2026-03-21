#include "config/config_env.h"
#include "config/defaults.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

static char *resolve_home(const char *suffix)
{
    const char *home = getenv("HOME");
    if (!home) {
        home = "/tmp";
    }

    size_t len = strlen(home) + strlen(suffix) + 1;
    char *path = malloc(len);
    if (!path) {
        return NULL;
    }

    (void)snprintf(path, len, "%s%s", home, suffix);
    return path;
}

static char *resolve_runtime(const char *suffix)
{
    uid_t uid = getuid();
    char buf[256];

    (void)snprintf(buf, sizeof(buf), "/run/user/%u%s", (unsigned)uid, suffix);
    return strdup(buf);
}

static char *env_or_home(const char *env_var, const char *default_suffix)
{
    const char *val = getenv(env_var);
    if (val) {
        return strdup(val);
    }
    return resolve_home(default_suffix);
}

static char *env_or_runtime(const char *env_var, const char *default_suffix)
{
    const char *val = getenv(env_var);
    if (val) {
        return strdup(val);
    }
    return resolve_runtime(default_suffix);
}

int fx_config_env_load(fx_config_t *cfg)
{
    cfg->watch_path  = env_or_home(FX_ENV_WATCH_PATH,   FX_DEFAULT_WATCH_PATH_SUFFIX);
    cfg->db_path     = env_or_home(FX_ENV_DB_PATH,      FX_DEFAULT_DB_PATH_SUFFIX);
    cfg->socket_path = env_or_runtime(FX_ENV_SOCKET_PATH, FX_DEFAULT_SOCKET_PATH_SUFFIX);

    if (!cfg->watch_path || !cfg->db_path || !cfg->socket_path) {
        free(cfg->watch_path);
        free(cfg->db_path);
        free(cfg->socket_path);
        cfg->watch_path = cfg->db_path = cfg->socket_path = NULL;
        return -1;
    }

    return 0;
}
