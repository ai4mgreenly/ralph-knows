#include "config/config.h"

#include <stdio.h>
#include <talloc.h>

int main(int argc, char **argv)
{
    TALLOC_CTX *ctx = talloc_new(NULL);

    res_t res = fx_cfg_load(ctx, argc, (const char **)(void *)argv);
    if (is_err(&res)) {
        fprintf(stderr, "fandex: %s\n", res.err->msg);
        talloc_free(ctx);
        return 1;
    }

    fx_cfg_t *cfg = res.ok;

    if (cfg->help) {
        printf("usage: fandex [--watch PATH] [--db PATH] [--socket PATH] [-h]\n"
               "\n"
               "  --watch PATH    directory to watch (default: %s)\n"
               "  --db PATH       database path (default: %s)\n"
               "  --socket PATH   socket path (default: %s)\n"
               "  -h, --help      show this help\n",
               cfg->watch_path, cfg->db_path, cfg->socket_path);
        talloc_free(ctx);
        return 0;
    }

    talloc_free(ctx);
    return 0;
}
