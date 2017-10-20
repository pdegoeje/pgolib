#ifndef RATIONAL_H
#define RATIONAL_H

#include <stddef.h>
#include <inttypes.h>

#ifdef __LP64__
typedef __int128 rat_num_t;
#else
typedef int64_t rat_num_t;
#endif

#define RAT_BITS (sizeof(rat_num_t) * 8)

rat_num_t chk_mul(rat_num_t a, rat_num_t b);
rat_num_t chk_add(rat_num_t a, rat_num_t b);
rat_num_t chk_sub(rat_num_t a, rat_num_t b);

typedef struct rational {
  rat_num_t numerator;
  rat_num_t divisor;
} rational_t;

rat_num_t rat_gcd(rat_num_t a, rat_num_t b);
rat_num_t rat_pow_s(rat_num_t x, rat_num_t y);
void rat_zero(rational_t *r, size_t count);
void rat_normalize(rational_t *r);
double rat_to_d(const rational_t *r) ;
void rat_add(rational_t *dst, const rational_t *inc);
void rat_add_fast(rational_t *dst, const rational_t *inc);
void rat_sub(rational_t *dst, const rational_t *inc);
void rat_mul_fast(rational_t *d, const rational_t *f);
void rat_mul(rational_t *d, const rational_t *f);
void rat_div(rational_t *d, const rational_t *f);
void rat_mul_s(rational_t *d, uint64_t s);
int rat_cmp(const rational_t *a, const rational_t *b);

/**
 * Find the LCM of all the divisors.
 */
rat_num_t rat_lcm(const rational_t *array, size_t count);

/**
 * Convert to string.
 */
char *rat_str(rat_num_t a);

#endif
