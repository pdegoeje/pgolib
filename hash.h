#ifndef HASH_H
#define HASH_H

#include "c_ext.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

//typedef struct list_node list_node_t;
typedef struct hash_table hash_table_t;

typedef uint32_t (*hash_func_t)(const void *key);
typedef bool (*compare_func_t)(const void *key1, const void *key2);

/**
 * \brief Points to the the list_node_t in the next node.
 * 
 * Each deference advances one node through the list.
 * 
 * The type is actually:
 * 
 *     typedef list_node_t *list_node_t;
 * 
 * But C doesn't allow recursive typedefs.
 * 
 * Thus, to traverse the list, use:
 * 
 *     for(list_node_t c = head; c; c = *(list_node_t *)c)
 *
 * c is a pointer to the node's list_node_t field.
 * 
 * or
 * 
 *     for(list_node_t *c = & head; *c; c = *c ) 
 * 
 * c is a pointer (reference) to the pointer to the node's list_node_t field.
 * 
 * The second version allows the list to be modified during traversal. No special
 * handling of the head pointer is necessary.
 * However it incurs an extra indirection (deference of c) so it is ever so 
 * slightly slower than the first version.
 */
typedef void *list_node_t;

struct hash_table {
  // Array of lists (hash chains).
  list_node_t *data;
  // Size of data (number of lists)
  size_t size;
  // Number of stored items in all lists.
  size_t count;
};

#define LIST_key_offset(container, node_member, key_member) \
  ((long)offsetof(container, key_member) - (long)offsetof(container, node_member))

#define LIST_exists(head_ptr) \
  (*(head_ptr) != NULL)
  
#define LIST_item(head_ptr, container, list_node) \
  (container_of(*(head_ptr), container, list_node))

/**
 * \brief Return a reference to the pointer to the next node.
 * 
 * \arg head_ptr
 *   Reference to the pointer to the current node.
 * 
 */
#define LIST_next(head_ptr) \
  ((list_node_t *)(*(head_ptr)))

#define LIST_next_direct(head) \
  (*(list_node_t *)(head))
  
/**
 * \brief Insert a node into a list.
 * 
 * \arg head_ptr
 *   Reference to the pointer to the node (may be NULL) before 
 *   which the list_node is to be inserted. To insert after a node,
 *   use that node's next pointer as a reference. 
 * 
 * \arg list_node
 *   list_node_t member of the node that is to be inserted. Must be an assignable (lvalue).
 * 
 * Example
 * <code>
 * typedef { list_node_t list; int data; } my_node_t;
 * 
 * list_node_t *list_head = ...;
 * my_node_t *node = ...;
 * 
 * // New list head.
 * LIST_insert(&list_head, &node->list);
 * 
 * my_node_t *another_node = ...;
 * // Insert after node.
 * LIST_insert(&node->list, &another_node->list);
 * </code>
 */
#define LIST_insert(head_ptr, list_node_ptr) \
  do { \
    *(list_node_ptr) = *(head_ptr); \
    *(head_ptr) = (list_node_ptr); \
  } while(0)
    
/**
 * \brief Remove a node from a list.
 * 
 * \arg head_ptr
 *   Reference to the pointer to the node to remove.
 */
#define LIST_remove(head_ptr) \
  do { \
    *(head_ptr) = *LIST_next(head_ptr); \
  } while(0)

/**
 * \brief Search a list for a node.
 * 
 * \arg head_ptr 
 *   List to search. Reference to the pointer to the first node.
 *   Will be updated to a reference to pointer to the result (which may be NULL)
 * 
 * \arg equals_func_ptr
 *   Signature: bool func(const void *node_key, const void *search_key). 
 *   Should return true if key equals the key to be tested.
 * 
 * \arg node_key_offset
 *   Distance in bytes between key member and node member. 
 *   Use LIST_key_offset() to calculate. If it is 0, the node_key is a pointer to
 *   the node's list_node_t field.
 * 
 * \arg key_ptr
 *   Key to search for.
 */
#define LIST_lookup(head_ptr, equals_func_ptr, node_key_offset, key_ptr) \
  while(LIST_exists(head_ptr)) { \
    if(equals_func_ptr((char *)*(head_ptr) + (node_key_offset), (key_ptr))) \
      break; \
    \
    (head_ptr) = LIST_next(head_ptr); \
  }
 
#define HASH_init(table_ptr, initial_size) \
  do { \
    (table_ptr)->size = (initial_size); \
    (table_ptr)->count = 0; \
    (table_ptr)->data = calloc(sizeof(list_node_t), (table_ptr)->size); \
  } while(0)

#define HASH_insert(table_ptr, head_ptr, list_node_ptr) \
  do { \
    LIST_insert(head_ptr, list_node_ptr); \
    (table_ptr)->count++; \
  } while(0)
  
#define HASH_remove(table_ptr, head_ptr) \
  do { \
    LIST_remove(head_ptr); \
    (table_ptr)->count--; \
  } while(0)
      
#define HASH_lookup(table_ptr, hash_func_ptr, key_ptr) \
  ((table_ptr)->data[(hash_func_ptr)(key_ptr) % (table_ptr)->size])

// FNV1-a (32 bit)
// http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param
  
#define FNV32_OFFSET 2166136261
#define FNV32_PRIME  16777619

#define FNV32_init(h) \
  h = FNV32_OFFSET

#define FNV32_add(h, v) \
  h = (h * FNV32_PRIME) ^ (v)
  
void hash_resize(hash_table_t *table_ptr, hash_func_t hash_func_ptr, long hash_key_offset, size_t newsize);  
  
#endif
