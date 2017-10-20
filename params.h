#ifndef PARAMS_H

#ifndef LOBYTE
#define LOBYTE(x) (x & 0xff)
#define HIBYTE(x) ((x >> 8) & 0xff)
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif


#ifndef MAKEWORD
#define MAKEWORD(lo, hi) ((lo & 0xFF) | (hi & 0xFF) << 8)
#endif

/* To be used ONLY for stuffing an int into a pointer, never the other way around */
#define INT_TO_PTR(a) ((void *)(long)(a))
#define PTR_TO_INT(a) ((int)(long)(a))

#ifndef nitems
#define nitems(array) (sizeof(array) / sizeof((array)[0]))
#endif

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

#define NORETURN __attribute__ ((noreturn))

#endif
