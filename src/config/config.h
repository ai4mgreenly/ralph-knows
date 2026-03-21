#ifndef FX_CONFIG_H
#define FX_CONFIG_H

#include <stdbool.h>

typedef struct fx_config {
    char *watch_path;
    char *db_path;
    char *socket_path;
    bool  help;
} fx_config_t;

// Load config: env vars override compiled-in defaults; CLI args override env.
//   FANDEX_WATCH_PATH  (default: ~/projects)
//   FANDEX_DB_PATH     (default: ~/.local/state/fandex/fandex.db)
//   FANDEX_SOCKET_PATH (default: /run/user/<uid>/fandex/fandex.sock)
//
// Returns NULL on allocation failure.
// Returns non-NULL with cfg->help set if --help/-h was passed.
// Caller must call fx_config_free() when done.
fx_config_t *fx_config_load(int argc, const char **argv);

void fx_config_free(fx_config_t *cfg);

#endif
