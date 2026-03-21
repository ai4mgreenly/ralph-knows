#include "config/config_args.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int fx_config_args_apply(fx_config_t *cfg, int argc, const char **argv)
{
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            cfg->help = true;
            continue;
        }

        if (strcmp(arg, "--watch") == 0 || strcmp(arg, "--db") == 0 ||
            strcmp(arg, "--socket") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "fandex: %s requires a value\n", arg);
                return 1;
            }
            const char *val = argv[++i];
            char *dup = strdup(val);
            if (!dup) {
                return -1;
            }

            if (strcmp(arg, "--watch") == 0) {
                free(cfg->watch_path);
                cfg->watch_path = dup;
            } else if (strcmp(arg, "--db") == 0) {
                free(cfg->db_path);
                cfg->db_path = dup;
            } else {
                free(cfg->socket_path);
                cfg->socket_path = dup;
            }
            continue;
        }

        fprintf(stderr, "fandex: unknown flag: %s\n", arg);
        return 1;
    }

    return 0;
}
