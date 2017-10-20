#include "hash.h"

#include <stdlib.h>

void hash_resize(hash_table_t *table_ptr, hash_func_t hash_func_ptr, long hash_key_offset, size_t newsize) { 
  list_node_t *olddata = table_ptr->data;
  size_t oldsize = table_ptr->size;

  list_node_t *newdata = calloc(sizeof(list_node_t), newsize);

  table_ptr->size = newsize;
  table_ptr->data = newdata;
  
  for(size_t i = 0; i < oldsize; i++) {
    for(list_node_t n, c = olddata[i]; c; c = n) {
      n = LIST_next_direct(c);
      size_t j = hash_func_ptr((char*)c + hash_key_offset) % newsize;
      LIST_next_direct(c) = newdata[j];
      newdata[j] = c;
    }
  }

  free(olddata);
} 
