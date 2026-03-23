#include "config/config.h"
#include <talloc.h>

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

static const char *no_args[] = {"ralph-knows"};

START_TEST(test_config_defaults) {
    unsetenv("RALPH_KNOWS_WATCH_PATH");
    unsetenv("RALPH_KNOWS_DB_PATH");
    unsetenv("RALPH_KNOWS_SOCKET_PATH");

    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 1, no_args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    const char *home = getenv("HOME");
    char expected[512];

    (void)snprintf(expected, sizeof(expected), "%s/projects", home);
    ck_assert_str_eq(cfg->watch_path, expected);

    (void)snprintf(expected, sizeof(expected), "%s/.local/state/ralph-knows/ralph-knows.db", home);
    ck_assert_str_eq(cfg->db_path, expected);

    uid_t uid = getuid();
    (void)snprintf(expected, sizeof(expected), "/run/user/%u/ralph-knows/ralph-knows.sock", uid);
    ck_assert_str_eq(cfg->socket_path, expected);

    talloc_free(ctx);
}
END_TEST

START_TEST(test_config_env_overrides) {
    setenv("RALPH_KNOWS_WATCH_PATH", "/mnt/data/projects", 1);
    setenv("RALPH_KNOWS_DB_PATH", "/tmp/test.db", 1);
    setenv("RALPH_KNOWS_SOCKET_PATH", "/tmp/test.sock", 1);

    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 1, no_args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->watch_path, "/mnt/data/projects");
    ck_assert_str_eq(cfg->db_path, "/tmp/test.db");
    ck_assert_str_eq(cfg->socket_path, "/tmp/test.sock");

    talloc_free(ctx);

    unsetenv("RALPH_KNOWS_WATCH_PATH");
    unsetenv("RALPH_KNOWS_DB_PATH");
    unsetenv("RALPH_KNOWS_SOCKET_PATH");
}
END_TEST

START_TEST(test_config_partial_overrides) {
    unsetenv("RALPH_KNOWS_WATCH_PATH");
    setenv("RALPH_KNOWS_DB_PATH", "/custom/path.db", 1);
    unsetenv("RALPH_KNOWS_SOCKET_PATH");

    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 1, no_args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    const char *home = getenv("HOME");
    char expected[512];

    (void)snprintf(expected, sizeof(expected), "%s/projects", home);
    ck_assert_str_eq(cfg->watch_path, expected);

    ck_assert_str_eq(cfg->db_path, "/custom/path.db");

    uid_t uid = getuid();
    (void)snprintf(expected, sizeof(expected), "/run/user/%u/ralph-knows/ralph-knows.sock", uid);
    ck_assert_str_eq(cfg->socket_path, expected);

    talloc_free(ctx);

    unsetenv("RALPH_KNOWS_DB_PATH");
}
END_TEST

START_TEST(test_config_args_win_over_env) {
    setenv("RALPH_KNOWS_WATCH_PATH", "/env/watch", 1);
    setenv("RALPH_KNOWS_DB_PATH", "/env/db.db", 1);
    setenv("RALPH_KNOWS_SOCKET_PATH", "/env/sock", 1);

    const char *args[] = {"ralph-knows", "--watch", "/arg/watch", "--db", "/arg/db.db", "--socket", "/arg/sock"};
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 7, args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->watch_path, "/arg/watch");
    ck_assert_str_eq(cfg->db_path, "/arg/db.db");
    ck_assert_str_eq(cfg->socket_path, "/arg/sock");

    talloc_free(ctx);

    unsetenv("RALPH_KNOWS_WATCH_PATH");
    unsetenv("RALPH_KNOWS_DB_PATH");
    unsetenv("RALPH_KNOWS_SOCKET_PATH");
}
END_TEST

START_TEST(test_config_args_win_over_defaults) {
    unsetenv("RALPH_KNOWS_WATCH_PATH");
    unsetenv("RALPH_KNOWS_DB_PATH");
    unsetenv("RALPH_KNOWS_SOCKET_PATH");

    const char *args[] = {"ralph-knows", "--watch", "/arg/watch", "--db", "/arg/db.db", "--socket", "/arg/sock"};
    TALLOC_CTX *ctx = talloc_new(NULL);
    res_t r = rk_cfg_load(ctx, 7, args);
    ck_assert(!r.is_err);
    rk_cfg_t *cfg = r.ok;
    ck_assert_ptr_nonnull(cfg);

    ck_assert_str_eq(cfg->watch_path, "/arg/watch");
    ck_assert_str_eq(cfg->db_path, "/arg/db.db");
    ck_assert_str_eq(cfg->socket_path, "/arg/sock");

    talloc_free(ctx);
}
END_TEST

static Suite *config_suite(void)
{
    Suite *s = suite_create("config");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_config_defaults);
    tcase_add_test(tc, test_config_env_overrides);
    tcase_add_test(tc, test_config_partial_overrides);
    tcase_add_test(tc, test_config_args_win_over_env);
    tcase_add_test(tc, test_config_args_win_over_defaults);
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
