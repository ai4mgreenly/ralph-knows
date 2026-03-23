#ifndef RK_WRAPPER_BASE_H
#define RK_WRAPPER_BASE_H

// MOCKABLE - link-seam mechanism for unit testing
//
// Release: static inline - real call is inlined, no link seam overhead
// Debug:   __attribute__((weak)) - tests can override with a strong symbol
#ifdef NDEBUG
#define MOCKABLE static inline
#else
#define MOCKABLE __attribute__((weak))
#endif

#endif // RK_WRAPPER_BASE_H
