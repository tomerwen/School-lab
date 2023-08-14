#ifndef __VECTOR_H_
#define __VECTOR_H_
#include <stddef.h>

struct vector;

typedef struct vector * Vector;

Vector new_vector(void * (*ctor)(const void *copy),void (*dtor)(void *item));/* allocates and returns a new Vector data structure.*/


void * vector_insert(Vector v,const void * copy_item);/*inserts a new item to the vector.*/


void *const *vector_begin(const Vector v);/*returns a pointer to the first pointer of the vector item list.*/

void *const *vector_end(const Vector v);/*returns a pointer to the last pointer of the vector item list*/


size_t vector_get_item_count(const Vector v);/*returns how many items are actually inside the vector*/

void vector_destroy(Vector * v);/*completely deallocates the vector, by calling the dtor function on each item + free'ing the memory of the vector*/



#define VECTOR_FOR_EACH(begin,end,v) for(begin   = vector_begin(v),end= vector_end(v);begin <= end;begin++)/*macro for iterating on a vector.*/

#define VECTOR_FOR_EACH_R(begin,end,v) for(begin   = vector_begin(v),end= vector_end(v);begin <= end;end--)/*macro for reverse iterating on a vector.*/


#endif