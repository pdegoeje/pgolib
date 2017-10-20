#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY(type) \
  struct { \
    type *data; \
    int count; \
    int capacity; \
  }
  
#define ARRAY_push(p, elem) \
  do { \
    if((p)->count == (p)->capacity) { \
      (p)->capacity = (p)->capacity < 2 ? 2 : (p)->capacity * 3 / 2; \
      (p)->data = realloc((p)->data, (p)->capacity * sizeof(*(p)->data)); \
    } \
    (p)->data[(p)->count++] = (elem); \
  } while(0)

#define ARRAY_pop(p) \
  ((p)->data[--(p)->count])
  
#define ARRAY_for_each_type(p, t, i) \
  for( t *i = (p)->data, *i##_end = (p)->data + (p)->count; i != i##_end; ++i)
  
#define ARRAY_for_each(p, i) \
  for( typeof ((p)->data) i = (p)->data, i##_end = (p)->data + (p)->count; i != i##_end; ++i)

#define ARRAY_for_each_offset(p, i, offset) \
  for( typeof ((p)->data) i = (p)->data + (offset), i##_end = (p)->data + (p)->count; i != i##_end; ++i)

#endif
