#include "config/config.h"
#include <talloc.h>

#include <check.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static void clear_env(void)
{
    unsetenv("RALPH_KNOWS_WATCH_PATH");
    unsetenv("RALPH_KNOWS_STATE_DIR");
    unsetenv("XDG_RUNTIME_DIR");
    unsetenv("RALPH_KNOWS_LOG_LEVEL");
}

// Tests use:
//   --watch /tmp        → instance_name = "tmp"
//   XDG_RUNTIME_DIR=/tmp → socket parent /tmp/ralph-knows (creatable)

START_TEST(test_config_defaults) {
    clear_env();
    setenv("XDG_RUNTIME_DIR", "/tmp", 1);

    const char *args[] = { "ralph-knows", "--watch", "/tmp" };
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 3, args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->watch_path, "/tmp");
    ck_assert_str_eq(cfg->instance_name, "tmp");
    ck_assert_str_eq(cfg->db_name, "tmp");

    const char *home = getenv("HOME");
    char expected[512];

    (void)snprintf(expected, sizeof(expected), "%s/.local/state/ralph-knows", home);
    ck_assert_str_eq(cfg->state_dir, expected);

    (void)snprintf(expected, sizeof(expected), "%s/.local/state/ralph-knows/tmp.db", home);
    ck_assert_str_eq(cfg->db_path, expected);

    ck_assert_str_eq(cfg->socket_path, "/tmp/ralph-knows/tmp.sock");

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_config_state_dir_env) {
    clear_env();
    setenv("RALPH_KNOWS_STATE_DIR", "/tmp/mystate", 1);
    setenv("XDG_RUNTIME_DIR", "/tmp", 1);

    const char *args[] = { "ralph-knows", "--watch", "/tmp" };
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 3, args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->state_dir, "/tmp/mystate");
    ck_assert_str_eq(cfg->db_path, "/tmp/mystate/tmp.db");

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_config_xdg_runtime_dir) {
    clear_env();
    setenv("XDG_RUNTIME_DIR", "/tmp", 1);
    setenv("RALPH_KNOWS_STATE_DIR", "/tmp/mystate", 1);

    const char *args[] = { "ralph-knows", "--watch", "/tmp" };
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 3, args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->socket_path, "/tmp/ralph-knows/tmp.sock");

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_config_db_override) {
    clear_env();
    setenv("RALPH_KNOWS_STATE_DIR", "/tmp/mystate", 1);
    setenv("XDG_RUNTIME_DIR", "/tmp", 1);

    const char *args[] = { "ralph-knows", "--watch", "/tmp", "--db", "custom" };
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 5, args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->db_name, "custom");
    ck_assert_str_eq(cfg->db_path, "/tmp/mystate/custom.db");
    ck_assert_str_eq(cfg->socket_path, "/tmp/ralph-knows/custom.sock");

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_config_db_slash_rejected) {
    clear_env();
    setenv("XDG_RUNTIME_DIR", "/tmp", 1);

    const char *args[] = { "ralph-knows", "--watch", "/tmp", "--db", "bad/name" };
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 5, args);
    ck_assert(r.is_err);

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_config_args_win_over_env) {
    setenv("RALPH_KNOWS_STATE_DIR", "/tmp/env-state", 1);
    setenv("XDG_RUNTIME_DIR", "/tmp", 1);

    const char *args[] = { "ralph-knows", "--watch", "/tmp", "--db", "override" };
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 5, args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->db_name, "override");
    ck_assert_str_eq(cfg->db_path, "/tmp/env-state/override.db");
    ck_assert_str_eq(cfg->socket_path, "/tmp/ralph-knows/override.sock");

    talloc_free(ctx);
    clear_env();
}
END_TEST

static Suite *config_suite(void)
{
    Suite *s = suite_create("config");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_config_defaults);
    tcase_add_test(tc, test_config_state_dir_env);
    tcase_add_test(tc, test_config_xdg_runtime_dir);
    tcase_add_test(tc, test_config_db_override);
    tcase_add_test(tc, test_config_db_slash_rejected);
    tcase_add_test(tc, test_config_args_win_over_env);
    suite_add_tcase(s, tc);

    return s;
}

int main(void)
{
    Suite *s = config_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, "reports/check/unit/config_test.xml");
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
