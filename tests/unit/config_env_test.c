#include "config/config_env.h"
#include "config/config.h"
#include "config/defaults.h"
#include "log/log.h"
#include <talloc.h>

#include <check.h>
#include <stdlib.h>

static void clear_env(void)
{
    unsetenv("RALPH_KNOWS_WATCH_PATH");
    unsetenv("RALPH_KNOWS_STATE_DIR");
    unsetenv("XDG_RUNTIME_DIR");
    unsetenv("RALPH_KNOWS_LOG_LEVEL");
}

START_TEST(test_env_no_vars) {
    clear_env();

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(!r.is_err);

    const char *home = getenv("HOME");
    char expected[512];

    (void)snprintf(expected, sizeof(expected), "%s%s", home, RK_DEFAULT_WATCH_PATH_SUFFIX);
    ck_assert_str_eq(cfg->watch_path, expected);

    (void)snprintf(expected, sizeof(expected), "%s%s", home, RK_DEFAULT_STATE_DIR_SUFFIX);
    ck_assert_str_eq(cfg->state_dir, expected);

    talloc_free(ctx);
}
END_TEST

START_TEST(test_env_watch_path) {
    clear_env();
    setenv("RALPH_KNOWS_WATCH_PATH", "/tmp/watch", 1);

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(!r.is_err);

    ck_assert_str_eq(cfg->watch_path, "/tmp/watch");

    const char *home = getenv("HOME");
    char expected[512];

    (void)snprintf(expected, sizeof(expected), "%s%s", home, RK_DEFAULT_STATE_DIR_SUFFIX);
    ck_assert_str_eq(cfg->state_dir, expected);

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_env_state_dir) {
    clear_env();
    setenv("RALPH_KNOWS_STATE_DIR", "/tmp/mystate", 1);

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(!r.is_err);

    ck_assert_str_eq(cfg->state_dir, "/tmp/mystate");

    const char *home = getenv("HOME");
    char expected[512];

    (void)snprintf(expected, sizeof(expected), "%s%s", home, RK_DEFAULT_WATCH_PATH_SUFFIX);
    ck_assert_str_eq(cfg->watch_path, expected);

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_env_both) {
    setenv("RALPH_KNOWS_WATCH_PATH", "/tmp/w", 1);
    setenv("RALPH_KNOWS_STATE_DIR", "/tmp/s", 1);

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(!r.is_err);

    ck_assert_str_eq(cfg->watch_path, "/tmp/w");
    ck_assert_str_eq(cfg->state_dir, "/tmp/s");

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_env_no_log_level) {
    clear_env();

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(!r.is_err);
    ck_assert_int_eq(cfg->log_level, RK_LOG_INFO);

    talloc_free(ctx);
}
END_TEST

START_TEST(test_env_log_level_warn) {
    clear_env();
    setenv("RALPH_KNOWS_LOG_LEVEL", "warn", 1);

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(!r.is_err);
    ck_assert_int_eq(cfg->log_level, RK_LOG_WARN);

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_env_log_level_debug) {
    clear_env();
    setenv("RALPH_KNOWS_LOG_LEVEL", "debug", 1);

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(!r.is_err);
    ck_assert_int_eq(cfg->log_level, RK_LOG_DEBUG);

    talloc_free(ctx);
    clear_env();
}
END_TEST

START_TEST(test_env_log_level_invalid) {
    clear_env();
    setenv("RALPH_KNOWS_LOG_LEVEL", "garbage", 1);

    TALLOC_CTX *ctx = talloc_new(NULL);
    rk_cfg_t *cfg = talloc_zero(ctx, rk_cfg_t);
    res_t r = rk_cfg_env_load(cfg);
    ck_assert(r.is_err);

    talloc_free(ctx);
    clear_env();
}
END_TEST

static Suite *config_env_suite(void)
{
    Suite *s = suite_create("config_env");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_env_no_vars);
    tcase_add_test(tc, test_env_watch_path);
    tcase_add_test(tc, test_env_state_dir);
    tcase_add_test(tc, test_env_both);
    tcase_add_test(tc, test_env_no_log_level);
    tcase_add_test(tc, test_env_log_level_warn);
    tcase_add_test(tc, test_env_log_level_debug);
    tcase_add_test(tc, test_env_log_level_invalid);
    suite_add_tcase(s, tc);

    return s;
}

int main(void)
{
    Suite *s = config_env_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, "reports/check/unit/config_env_test.xml");
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
