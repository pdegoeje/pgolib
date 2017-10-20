#include "c_ext.h"
#include "pcg.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#ifdef WIN32
#include <windows.h>
#endif

// Based on PCG random alorithm. See pcg-random.org
uint32_t pcg_next(uint64_t *state) {
  uint64_t oldstate = *state;
  *state = oldstate * 6364136223846793005ULL + 1;
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint64_t pcg_uniform(uint64_t *state, uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint32_t upper = pcg_next(state);
    uint32_t lower = pcg_next(state);
    uint64_t r = (uint64_t)upper << 32 | lower;
    if (r >= threshold)
      return r % bound;
  }
}

void pcg_seed(uint64_t *state) {
#ifdef WIN32
  HCRYPTPROV prov;
  if(CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE) {
    panic("Unable to acquire crypto context"); 
  }
  
  if(CryptGenRandom(prov, sizeof *state, (BYTE *)state) == FALSE) {
    panic("Unable to generate random data");
  }
  
  CryptReleaseContext(prov, 0);
#else
  int fd = open("/dev/random", O_RDONLY);
  if(fd == -1) {
    panic("Unable to open /dev/random");
  }
  
  read(fd, state, sizeof *state);
  close(fd);
#endif
}

