#include "bin_coeff.h"

#include <stdlib.h>
#include <assert.h>

static const int maxN = 66; // Max for 64bit. This causes 1024 words to be allocated.

static int64_t *pascal; // Pascal's triangle, compressed.

/**
 * Calculate the table offset.
 */
static int bin_coeff_off(int n, int k) {
  int np = n - 3;
  return ((np * np) / 4) + (k - 2);
}

/**
 * Calculate the binomial coefficient of (n, k).
 * Note that this is the same as "Combinations", aka n Choose k
 */
int64_t bin_coeff(int n, int k) {
  assert(n >= 0);
  assert(n <= maxN);
  assert(k >= 0);
  assert(k <= n);
  
  int half = n - k;
  if(k > half) {
    k = half;
  }
  
  if(k == 0)
    return 1;
  
  if(k == 1)
    return n;
  
  return pascal[bin_coeff_off(n, k)];
}

void bin_coeff_init() {
  int cache_size = bin_coeff_off(maxN + 1, 2);
  pascal = (int64_t *)malloc(cache_size * sizeof (int64_t));
  
  for(int n = 4; n <= maxN; n++) {
    for(int k = 2; k <= n / 2; k++) {
      pascal[bin_coeff_off(n, k)] = 
        bin_coeff(n - 1, k - 1) + 
        bin_coeff(n - 1, k);
    }
  }
}

#if 0
rat_num_t combinations(int n, int k) {
  assert(n >= k);
  
  // n Choose k == n Choose (n - k)
  
  if(k > (n / 2)) {
    k = n - k;
  }
  
  
  rat_num_t b = 1;
  rat_num_t n_k = n - k;
  for (rat_num_t i = 1; i <= k; ++i) {
    b *= n_k + i; //(n - (k - i));
    b /= i;
  }
  
  return b;
}
#endif