#ifndef C_EXT_H
#define C_EXT_H

#include <stdint.h>

#define UNREACHABLE  __builtin_unreachable()
#define NORETURN     __attribute__ ((noreturn))
#define NOINLINE     __attribute__ ((noinline))

#define container_of(ptr, type, member) (type *)((intptr_t)(ptr) - offsetof(type, member))

void panic(const char *fmt, ...) NORETURN;

#endif
