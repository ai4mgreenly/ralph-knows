#include "log/log.h"

#include "error.h"

#include "poison.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

struct rk_log {
    FILE *out;
    rk_log_level_t level;
    pthread_mutex_t mutex;
};

// Talloc destructor: releases the mutex before the struct is freed.
static int log_destructor(rk_log_t *log)
{
    pthread_mutex_destroy(&log->mutex);
    return 0;
}

rk_log_t *rk_log_init(TALLOC_CTX *ctx, FILE *out, rk_log_level_t level)
{
    rk_log_t *log = talloc_zero(ctx, rk_log_t);
    if (!log) PANIC("Out of memory"); // LCOV_EXCL_BR_LINE

    log->out = out;
    log->level = level;
    pthread_mutex_init(&log->mutex, NULL);
    talloc_set_destructor(log, log_destructor);
    return log;
}

void rk_log_info(rk_log_t *log, const char *fmt, ...)
{
    if (RK_LOG_INFO < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s INFO ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
}

void rk_log_warn(rk_log_t *log, const char *fmt, ...)
{
    if (RK_LOG_WARN < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s WARN ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
}

void rk_log_error(rk_log_t *log, const char *fmt, ...)
{
    if (RK_LOG_ERROR < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s ERROR ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
}

#ifdef DEBUG
void rk_log_debug(rk_log_t *log, const char *fmt, ...)
{
    if (RK_LOG_DEBUG < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s DEBUG ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
}

#endif
