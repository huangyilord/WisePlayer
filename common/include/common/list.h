//
//  list.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _LIST_H_ )
#define _LIST_H_

#include "object_pool.h"

typedef struct list_node
{
    struct list_node*       prev;
    struct list_node*       next;
} list_node;

typedef struct list
{
    object_pool     node_pool;
    list_node*      first;
    list_node*      last;
    size_t          element_size;
    uint32          size;
} list;

/** Initialize a list instance.
 *
 *  @param list:            The list instance.
 *  @param pool:            An initialized memory_pool instance.
 *  @return:                Returns TRUE if success.
 */
boolean list_initialize( list* plist, size_t element_size, memory_pool* pool );

/** Destroy a list instance.
 *
 *  @param list:            The list instance.
 *  @return:                None.
 */
void list_destroy( list* plist );

/** Put an object at the beginning of the list.
 *
 *  @param list:            The list instance.
 *  @param value:           The object to be push.
 *  @return:                Return TRUE if success.
 */
boolean list_push_front( list* plist, const void* value );

/** Remove the frist object from the list.
 *
 *  @param list:            The list instance.
 *  @return:                None.
 */
void list_pop_front( list* plist );

/** Get the first object of the list.
 *
 *  @param list:            The list instance.
 *  @return:                The pointer to the first object.
 *                          If the list is empty, return NULL.
 */
void* list_front( list* plist );

/** Put an object at the tail of the list.
 *
 *  @param list:            The list instance.
 *  @param value:           The object to be push.
 *  @return:                Return TRUE if success.
 */
boolean list_push_back( list* plist, const void* value );

/** Remove the last object from the list.
 *
 *  @param list:            The list instance.
 *  @return:                None.
 */
void list_pop_back( list* plist );

/** Get the last object of the list.
 *
 *  @param list:            The list instance.
 *  @return:                The pointer to the last object.
 *                          If the list is empty, return NULL.
 */
void* list_back( list* plist );

/** Remove all the content from the list.
 *
 *  @param list:            The list instance.
 *  @return:                None.
 */
void list_clear( list* plist );


typedef list_node*  list_iterator;

#define LIST_SIZE( list ) \
    ((list)->size)

#define LIST_IS_EMPTY( list ) \
    ( (list)->size == 0 )

#define LIST_ITERATOR_START( iter, list ) \
    ((iter) = (list)->first)

#define LIST_ITERATOR_IS_VALID( iter ) \
    ((iter) != NULL)

#define LIST_ITERATOR_NEXT( iter ) \
    ((iter) = iter->next)

#define LIST_ITERATOR_GET( iter ) \
    ((void*)(iter + 1))

void list_remove( list* plist, list_iterator iter );

typedef list_node*  list_riterator;

#define LIST_RITERATOR_START( iter, list ) \
    ((iter) = (list)->last)

#define LIST_RITERATOR_IS_VALID( iter ) \
    ((iter) != NULL)

#define LIST_RITERATOR_NEXT( iter ) \
    ((iter) = iter->prev)

#define LIST_RITERATOR_GET( iter ) \
    ((void*)(iter + 1))

#endif
