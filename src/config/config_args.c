#include "config/config_args.h"
#include <talloc.h>

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

        if (strcmp(arg, "--watch") == 0) {
            if (i + 1 >= argc) {
                return ERR(cfg, INVALID_ARG, "ralph-knows: %s requires a value", arg);
            }
            const char *val = argv[++i];
            char *dup = talloc_strdup(cfg, val);
            if (!dup) {
                PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
            }
            talloc_free(cfg->watch_path);
            cfg->watch_path = dup;
            continue;
        }

        if (strcmp(arg, "--db") == 0) {
            if (i + 1 >= argc) {
                return ERR(cfg, INVALID_ARG, "ralph-knows: %s requires a value", arg);
            }
            const char *val = argv[++i];
            if (strchr(val, '/') != NULL) {
                return ERR(cfg, INVALID_ARG,
                           "ralph-knows: --db takes a bare filename (no slashes): %s", val);
            }
            char *dup = talloc_strdup(cfg, val);
            if (!dup) {
                PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
            }
            talloc_free(cfg->db_name);
            cfg->db_name = dup;
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
