#pragma once

#define FAIL                                                                  \
    do {                                                                      \
        fprintf(stderr, "ERROR! %s: %s: %d\n", __FILE__, __func__, __LINE__); \
        exit(EXIT_FAILURE);                                                   \
    } while (0)

#define FAILARG(A)                                                                        \
    do {                                                                                  \
        fprintf(stderr, "ERROR! %s: %s: %d. MSG: %s\n", __FILE__, __func__, __LINE__, A); \
        exit(EXIT_FAILURE);                                                               \
    } while (0)

inline void LOG_DEBUG(const char* str) {
    if (LOG_ON_OFF)
        fprintf(stderr, "%s\n", str);
}
inline void LOG_DEBUG(const char* str, int arg) {
    if (LOG_ON_OFF)
        fprintf(stderr, "%s: %d\n", str, arg);
}

