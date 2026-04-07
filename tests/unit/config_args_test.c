#include "config/config_args.h"
#include "config/config_env.h"
#include "config/config.h"
#include "log/log.h"
#include <talloc.h>

#include <check.h>
#include <stdlib.h>
#include <string.h>

/* Helper: load a fresh cfg under a new top-level talloc context. */
static rk_cfg_t *base_cfg(void)
{
    unsetenv("RALPH_KNOWS_WATCH_PATH");
    unsetenv("RALPH_KNOWS_STATE_DIR");
    unsetenv("RALPH_KNOWS_LOG_LEVEL");
    rk_cfg_t *cfg = talloc_zero(NULL, rk_cfg_t);
    if (cfg) {
        rk_cfg_env_load(cfg);
    }
    return cfg;
}

START_TEST(test_args_empty) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    char *w = talloc_strdup(cfg, cfg->watch_path);
    char *s = talloc_strdup(cfg, cfg->state_dir);

    const char *argv[] = { "ralph-knows" };
    res_t r = rk_cfg_args_apply(cfg, 1, argv);

    ck_assert(!r.is_err);
    ck_assert(!cfg->help);
    ck_assert_str_eq(cfg->watch_path, w);
    ck_assert_str_eq(cfg->state_dir, s);
    ck_assert_ptr_null(cfg->db_name);

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_help_long) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--help" };
    res_t r = rk_cfg_args_apply(cfg, 2, argv);

    ck_assert(!r.is_err);
    ck_assert(cfg->help);

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_help_short) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "-h" };
    res_t r = rk_cfg_args_apply(cfg, 2, argv);

    ck_assert(!r.is_err);
    ck_assert(cfg->help);

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_watch) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--watch", "/tmp/w" };
    res_t r = rk_cfg_args_apply(cfg, 3, argv);

    ck_assert(!r.is_err);
    ck_assert_str_eq(cfg->watch_path, "/tmp/w");

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_db_bare) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--db", "mydb" };
    res_t r = rk_cfg_args_apply(cfg, 3, argv);

    ck_assert(!r.is_err);
    ck_assert_str_eq(cfg->db_name, "mydb");

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_db_slash_rejected) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--db", "bad/name" };
    res_t r = rk_cfg_args_apply(cfg, 3, argv);

    ck_assert(r.is_err);

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_watch_and_db) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = {
        "ralph-knows",
        "--watch", "/tmp/w",
        "--db", "mydb"
    };
    res_t r = rk_cfg_args_apply(cfg, 5, argv);

    ck_assert(!r.is_err);
    ck_assert_str_eq(cfg->watch_path, "/tmp/w");
    ck_assert_str_eq(cfg->db_name, "mydb");

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_unknown_flag) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--bogus" };
    res_t r = rk_cfg_args_apply(cfg, 2, argv);

    ck_assert(r.is_err);

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_missing_value) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--watch" };
    res_t r = rk_cfg_args_apply(cfg, 2, argv);

    ck_assert(r.is_err);

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_log_level_error) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--log-level", "error" };
    res_t r = rk_cfg_args_apply(cfg, 3, argv);

    ck_assert(!r.is_err);
    ck_assert_int_eq(cfg->log_level, RK_LOG_ERROR);

    rk_cfg_free(cfg);
}
END_TEST

START_TEST(test_args_log_level_missing_value) {
    rk_cfg_t *cfg = base_cfg();
    ck_assert_ptr_nonnull(cfg);

    const char *argv[] = { "ralph-knows", "--log-level" };
    res_t r = rk_cfg_args_apply(cfg, 2, argv);

    ck_assert(r.is_err);

    rk_cfg_free(cfg);
}
END_TEST

static Suite *config_args_suite(void)
{
    Suite *s = suite_create("config_args");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_args_empty);
    tcase_add_test(tc, test_args_help_long);
    tcase_add_test(tc, test_args_help_short);
    tcase_add_test(tc, test_args_watch);
    tcase_add_test(tc, test_args_db_bare);
    tcase_add_test(tc, test_args_db_slash_rejected);
    tcase_add_test(tc, test_args_watch_and_db);
    tcase_add_test(tc, test_args_unknown_flag);
    tcase_add_test(tc, test_args_missing_value);
    tcase_add_test(tc, test_args_log_level_error);
    tcase_add_test(tc, test_args_log_level_missing_value);
    suite_add_tcase(s, tc);

    return s;
}

int main(void)
{
    Suite *s = config_args_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, "reports/check/unit/config_args_test.xml");
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
