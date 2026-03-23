#include "log/log.h"
#include <talloc.h>

#include <check.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Read all content written to a tmpfile into a static buffer. */
static char buf[1024];

static const char *read_tmpfile(FILE *f)
{
    rewind(f);
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    buf[n] = '\0';
    /* Reset for the next write. */
    rewind(f);
    (void)ftruncate(fileno(f), 0);
    return buf;
}

START_TEST(test_log_info_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_DEBUG);
    rk_log_info(log, "hello");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " INFO "));
    ck_assert_ptr_nonnull(strstr(out, "hello"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_warn_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_DEBUG);
    rk_log_warn(log, "something");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " WARN "));
    ck_assert_ptr_nonnull(strstr(out, "something"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_error_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_DEBUG);
    rk_log_error(log, "boom");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " ERROR "));
    ck_assert_ptr_nonnull(strstr(out, "boom"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_format_args) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_DEBUG);
    rk_log_info(log, "count=%" PRId32, (int32_t)42);

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, "count=42"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_level_suppresses_info) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_WARN);
    rk_log_info(log, "suppressed");

    const char *out = read_tmpfile(f);
    ck_assert_str_eq(out, "");

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_level_suppresses_warn) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_ERROR);
    rk_log_warn(log, "suppressed");

    const char *out = read_tmpfile(f);
    ck_assert_str_eq(out, "");

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_debug_level_allows_info) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_DEBUG);
    rk_log_info(log, "visible");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, "visible"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

#define TS_NUM_THREADS 8
#define TS_MSGS_PER_THREAD 100

struct ts_thread_arg {
    rk_log_t *log;
    int32_t id;
};

static void *ts_log_thread(void *arg)
{
    struct ts_thread_arg *a = arg;
    for (int32_t i = 0; i < TS_MSGS_PER_THREAD; i++) {
        rk_log_info(a->log, "thread=%" PRId32 " seq=%" PRId32, a->id, i);
    }
    return NULL;
}

START_TEST(test_log_thread_safety) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_INFO);

    pthread_t threads[TS_NUM_THREADS];
    struct ts_thread_arg args[TS_NUM_THREADS];
    for (int32_t i = 0; i < TS_NUM_THREADS; i++) {
        args[i].log = log;
        args[i].id = i;
        pthread_create(&threads[i], NULL, ts_log_thread, &args[i]);
    }
    for (int32_t i = 0; i < TS_NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    fflush(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *out = malloc((size_t)size + 1);
    ck_assert_ptr_nonnull(out);
    size_t n = fread(out, 1, (size_t)size, f);
    out[n] = '\0';

    int32_t line_count = 0;
    char *p = out;
    while (*p) {
        /* Each line must start with a digit (timestamp). */
        ck_assert(p[0] >= '0' && p[0] <= '9');
        char *nl = strchr(p, '\n');
        ck_assert_ptr_nonnull(nl);
        /* No embedded INFO/WARN/ERROR markers within a single line body. */
        *nl = '\0';
        ck_assert_ptr_nonnull(strstr(p, " INFO "));
        *nl = '\n';
        line_count++;
        p = nl + 1;
    }
    ck_assert_int_eq(line_count, TS_NUM_THREADS * TS_MSGS_PER_THREAD);

    free(out);
    fclose(f);
    talloc_free(ctx);
}
END_TEST

#ifdef DEBUG
START_TEST(test_log_debug_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_DEBUG);
    rk_log_debug(log, "trace");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " DEBUG "));
    ck_assert_ptr_nonnull(strstr(out, "trace"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_debug_suppressed_by_info_level) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    rk_log_t *log = rk_log_init(ctx, f, RK_LOG_INFO);
    rk_log_debug(log, "suppressed");

    const char *out = read_tmpfile(f);
    ck_assert_str_eq(out, "");

    fclose(f);
    talloc_free(ctx);
}
END_TEST
#endif /* DEBUG */

static Suite *log_suite(void)
{
    Suite *s = suite_create("log");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_log_info_prefix);
    tcase_add_test(tc, test_log_warn_prefix);
    tcase_add_test(tc, test_log_error_prefix);
    tcase_add_test(tc, test_log_format_args);
    tcase_add_test(tc, test_log_level_suppresses_info);
    tcase_add_test(tc, test_log_level_suppresses_warn);
    tcase_add_test(tc, test_log_debug_level_allows_info);
    tcase_add_test(tc, test_log_thread_safety);
#ifdef DEBUG
    tcase_add_test(tc, test_log_debug_prefix);
    tcase_add_test(tc, test_log_debug_suppressed_by_info_level);
#endif

    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    Suite *s = log_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, "reports/check/unit/log_test.xml");
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
