#include "rational.h"
#include "c_ext.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __clang__
  #if !__has_builtin(__builtin_mul_overflow)
    #warning "Compiler does not support generic overflow checking"
    #ifdef __i386__
      #define __builtin_mul_overflow __builtin_smulll_overflow
      #define __builtin_add_overflow __builtin_saddll_overflow
      #define __builtin_sub_overflow __builtin_ssubll_overflow
    #else
      #define __builtin_mul_overflow __builtin_smull_overflow
      #define __builtin_add_overflow __builtin_saddl_overflow
      #define __builtin_sub_overflow __builtin_ssubl_overflow
    #endif
  #endif
#endif

#define CHK_BUILTIN 1
#define CHK_NONE    2
#define CHK_MANUAL  3

#ifndef CHK_METHOD
#define CHK_METHOD  CHK_BUILTIN
#endif

rat_num_t chk_mul(rat_num_t a, rat_num_t b) {
#if CHK_METHOD == CHK_BUILTIN
  rat_num_t result;
  if(__builtin_mul_overflow(a, b, &result))
    panic("Multiplication overflow");
  
  return result;
#elif CHK_METHOD == CHK_NONE
  return a * b;
#elif CHK_METHOD == CHK_MANUAL
  rat_num_t result = a * b;
  if(result / b != a)
    panic("Multiplication overflow");
  return result;
#endif
}

rat_num_t chk_add(rat_num_t a, rat_num_t b) {
#if CHK_METHOD == CHK_BUILTIN
  rat_num_t result;
  if(__builtin_add_overflow(a, b, &result))
    panic("Addition overflow");
  
  return result;
#elif CHK_METHOD == CHK_NONE
  return a + b;
#elif CHK_METHOD == CHK_MANUAL
  if(((unsigned rat_num_t)a >> (RAT_BITS - 1)) && ((unsigned rat_num_t)b >> (RAT_BITS - 1)))
    panic("Addition overflow");
  rat_num_t result = a + b;
  return result;
#endif
}

rat_num_t chk_sub(rat_num_t a, rat_num_t b) {
#if CHK_METHOD == CHK_BUILTIN
  rat_num_t result;
  if(__builtin_sub_overflow(a, b, &result))
    panic("Subtraction overflow");
  
  return result;
#elif CHK_METHOD == CHK_NONE
  return a - b;
#elif CHK_METHOD == CHK_MANUAL
  return a - b;
#endif
}

char *rat_str(rat_num_t a) {
  static char buf[40];
  
  char *p = buf + 39;
  *p-- = '\0';
  do {
    int rest = a % 10;
    a /= 10;
    *p-- = rest + '0';
  } while(a);
  return p + 1;
}

rat_num_t rat_gcd(rat_num_t a, rat_num_t b) {
  while (b != 0) {
    rat_num_t t = b;
    b = a % b;
    a = t;
  }
  return a;
}

rat_num_t rat_pow_s(rat_num_t x, rat_num_t y) {
  assert(y >= 0);
  
  rat_num_t result = 1;
  while (y)  {
    if (y & 1) {
      result = chk_mul(result, x);
    }
    
    y >>= 1;
    
    if(!y)
      break;
    
    x = chk_mul(x, x);
  }

  return result;
}

void rat_zero(rational_t *r, size_t count) {
  for(size_t i = 0; i < count; i++) {
    r[i].numerator = 0;
    r[i].divisor   = 1;
  }
}

void rat_normalize(rational_t *r) {
  rat_num_t g = rat_gcd(r->numerator, r->divisor);
  r->numerator /= g;
  r->divisor   /= g;
}

double rat_to_d(const rational_t *r) {
  return (double)r->numerator / (double)r->divisor;
}

void rat_add(rational_t *dst, const rational_t *inc) {
  if(dst->divisor != inc->divisor) {
    rat_num_t g = rat_gcd(dst->divisor, inc->divisor);
    rat_num_t dst_fac = inc->divisor / g;
    rat_num_t inc_fac = dst->divisor / g;
    
    dst->numerator = chk_add(chk_mul(dst->numerator, dst_fac), chk_mul(inc->numerator, inc_fac));
    dst->divisor = chk_mul(dst->divisor, dst_fac);
  } else {
    dst->numerator = chk_add(dst->numerator, inc->numerator);
  }
  
  rat_normalize(dst);
}

void rat_add_fast(rational_t *dst, const rational_t *inc) {
  if(dst->divisor != inc->divisor) {
    rat_num_t g = rat_gcd(dst->divisor, inc->divisor);
    rat_num_t dst_fac = inc->divisor / g;
    rat_num_t inc_fac = dst->divisor / g;
    dst->numerator = chk_add(chk_mul(dst->numerator, dst_fac), chk_mul(inc->numerator, inc_fac));
    dst->divisor = chk_mul(dst->divisor, dst_fac);
  } else {
    dst->numerator = chk_add(dst->numerator, inc->numerator);
  }
}


void rat_sub(rational_t *dst, const rational_t *inc) {
  rat_num_t g = rat_gcd(dst->divisor, inc->divisor);
  rat_num_t dst_fac = inc->divisor / g;
  rat_num_t inc_fac = dst->divisor / g;
  // Use least common multiple.
  dst->numerator = chk_sub(chk_mul(dst->numerator, dst_fac), chk_mul(inc->numerator, inc_fac));
  dst->divisor = chk_mul(dst->divisor, dst_fac);
  
  rat_normalize(dst);
}

void rat_mul(rational_t *d, const rational_t *f) {
  d->numerator = chk_mul(d->numerator, f->numerator);
  d->divisor   = chk_mul(d->divisor,   f->divisor);
  
  rat_normalize(d);
}

void rat_mul_fast(rational_t *d, const rational_t *f) {
  d->numerator = chk_mul(d->numerator, f->numerator);
  d->divisor   = chk_mul(d->divisor,   f->divisor);
}

void rat_div(rational_t *d, const rational_t *f) {
  d->numerator = chk_mul(d->numerator, f->divisor);
  d->divisor =   chk_mul(d->divisor, f->numerator);
  
  rat_normalize(d);
}

void rat_mul_s(rational_t *d, uint64_t s) {
  // Todo use GCD to avoid overflow
  d->numerator = chk_mul(d->numerator, s);
  rat_normalize(d); 
}

int rat_cmp(const rational_t *a, const rational_t *b) {
  rat_num_t g = rat_gcd(a->divisor, b->divisor);
  rat_num_t a_fac = b->divisor / g;
  rat_num_t b_fac = a->divisor / g;
  
  rat_num_t an = chk_mul(a->numerator, a_fac);
  rat_num_t bn = chk_mul(b->numerator, b_fac);
  
  return an > bn ? +1 : 
         bn > an ? -1 : 
                    0;  
}

/**
 * Find the LCM of all the divisors.
 */
rat_num_t rat_lcm(const rational_t *array, size_t count) {
  rat_num_t lcm = array[0].divisor;
  for(size_t i = 1; i < count; i++) {
    rat_num_t g = rat_gcd(lcm, array[i].divisor);
    lcm = chk_mul(lcm, array[i].divisor / g);
  }
  return lcm;
}
