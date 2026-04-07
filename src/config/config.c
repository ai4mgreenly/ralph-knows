#include "config/config.h"
#include "config/config_args.h"
#include "config/config_env.h"
#include <talloc.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "poison.h"

res_t rk_cfg_load(TALLOC_CTX *ctx, int argc, const char **argv)
{
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    if (!cfg) {
        PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
    }

    res_t r = rk_cfg_env_load(cfg);
    if (is_err(&r)) {
        talloc_steal(ctx, r.err);
        talloc_free(cfg);
        return r;
    }

    r = rk_cfg_args_apply(cfg, argc, argv);
    if (is_err(&r)) {
        talloc_steal(ctx, r.err);
        talloc_free(cfg);
        return r;
    }

    // Derive instance_name = basename(realpath(watch_path))
    char *canonical = realpath(cfg->watch_path, NULL);
    if (!canonical) {
        res_t err_res = ERR(ctx, IO, "ralph-knows: cannot resolve watch path '%s': %s",
                            cfg->watch_path, strerror(errno));
        talloc_free(cfg);
        return err_res;
    }
    const char *base = strrchr(canonical, '/');
    base = (base && base[1] != '\0') ? base + 1 : canonical;
    cfg->instance_name = talloc_strdup(cfg, base);
    free(canonical);
    if (!cfg->instance_name) {
        PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
    }

    // db_name: use --db override or fall back to instance_name
    if (!cfg->db_name) {
        cfg->db_name = talloc_strdup(cfg, cfg->instance_name);
        if (!cfg->db_name) {
            PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
        }
    }

    // db_path = state_dir/db_name.db
    cfg->db_path = talloc_asprintf(cfg, "%s/%s.db", cfg->state_dir, cfg->db_name);
    if (!cfg->db_path) {
        PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
    }

    // socket_path = XDG_RUNTIME_DIR/ralph-knows/db_name.sock
    const char *xdg = getenv("XDG_RUNTIME_DIR");
    if (xdg) {
        cfg->socket_path = talloc_asprintf(cfg, "%s/ralph-knows/%s.sock", xdg, cfg->db_name);
    } else {
        uid_t uid = getuid();
        cfg->socket_path = talloc_asprintf(cfg, "/run/user/%u/ralph-knows/%s.sock",
                                           (unsigned)uid, cfg->db_name);
    }
    if (!cfg->socket_path) {
        PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
    }

    // Create state_dir, creating one parent level if needed
    {
        char *parent = talloc_strdup(cfg, cfg->state_dir);
        if (!parent) {
            PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
        }
        char *slash = strrchr(parent, '/');
        if (slash && slash != parent) {
            *slash = '\0';
            (void)mkdir(parent, 0755);
        }
        talloc_free(parent);
    }
    if (mkdir(cfg->state_dir, 0700) != 0 && errno != EEXIST) {
        res_t err_res = ERR(ctx, IO, "ralph-knows: cannot create state dir '%s': %s",
                            cfg->state_dir, strerror(errno));
        talloc_free(cfg);
        return err_res;
    }

    // Create socket parent dir (XDG_RUNTIME_DIR/ralph-knows or /run/user/UID/ralph-knows)
    {
        char *sock_parent = talloc_strdup(cfg, cfg->socket_path);
        if (!sock_parent) {
            PANIC("Out of memory"); // LCOV_EXCL_BR_LINE
        }
        char *slash = strrchr(sock_parent, '/');
        if (slash) {
            *slash = '\0';
        }
        if (mkdir(sock_parent, 0700) != 0 && errno != EEXIST) {
            res_t err_res = ERR(ctx, IO, "ralph-knows: cannot create socket dir '%s': %s",
                                sock_parent, strerror(errno));
            talloc_free(cfg);
            return err_res;
        }
        talloc_free(sock_parent);
    }

    return OK(cfg);
}

void rk_cfg_free(rk_cfg_t *cfg)
{
    talloc_free(cfg);
}
