#ifndef RK_LOG_H
#define RK_LOG_H

#include <stdio.h>
#include <talloc.h>

typedef enum {
    RK_LOG_DEBUG = 0,
    RK_LOG_INFO,
    RK_LOG_WARN,
    RK_LOG_ERROR,
} rk_log_level_t;

typedef struct rk_log rk_log_t;

// Allocate a logger on ctx writing to out at the given level.
// OOM calls PANIC(). Caller owns the returned pointer (freed with ctx).
rk_log_t *rk_log_init(TALLOC_CTX *ctx, FILE *out, rk_log_level_t level);

void rk_log_info(rk_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void rk_log_warn(rk_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void rk_log_error(rk_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#ifdef DEBUG
void rk_log_debug(rk_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#else
#define rk_log_debug(log, ...) ((void)0)
#endif

#endif // RK_LOG_H
