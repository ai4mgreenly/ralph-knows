#include "config/config_args.h"
#include <talloc.h>

#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "poison.h"

res_t rk_cfg_args_apply(rk_cfg_t *cfg, int argc, const char **argv)
{
    for (int32_t i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            cfg->help = true;
            continue;
        }

        if (strcmp(arg, "--watch") == 0 || strcmp(arg, "--db") == 0 ||
            strcmp(arg, "--socket") == 0) {
            if (i + 1 >= argc) {
                return ERR(cfg, INVALID_ARG, "ralph-knows: %s requires a value", arg);
            }
            const char *val = argv[++i];
            char *dup = talloc_strdup(cfg, val);
            if (!dup) {
                PANIC("Out of memory");
            }

            if (strcmp(arg, "--watch") == 0) {
                talloc_free(cfg->watch_path);
                cfg->watch_path = dup;
            } else if (strcmp(arg, "--db") == 0) {
                talloc_free(cfg->db_path);
                cfg->db_path = dup;
            } else {
                talloc_free(cfg->socket_path);
                cfg->socket_path = dup;
            }
            continue;
        }

        if (strcmp(arg, "--log-level") == 0) {
            if (i + 1 >= argc) {
                return ERR(cfg, INVALID_ARG, "ralph-knows: --log-level requires a value");
            }
            const char *val = argv[++i];
            if (strcasecmp(val, "debug") == 0) {
                cfg->log_level = RK_LOG_DEBUG;
            } else if (strcasecmp(val, "info") == 0) {
                cfg->log_level = RK_LOG_INFO;
            } else if (strcasecmp(val, "warn") == 0) {
                cfg->log_level = RK_LOG_WARN;
            } else if (strcasecmp(val, "error") == 0) {
                cfg->log_level = RK_LOG_ERROR;
            } else {
                return ERR(cfg, INVALID_ARG, "ralph-knows: invalid --log-level: %s", val);
            }
            continue;
        }

        return ERR(cfg, INVALID_ARG, "ralph-knows: unknown flag: %s", arg);
    }

    return OK(NULL);
}
