//
//  array_list.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _NOTIFICATION_ARRAY_LIST_H_ )
#define _NOTIFICATION_ARRAY_LIST_H_

#include "memory_pool.h"

typedef struct array_list_block_info
{
    void*           prev_block;
    void*           next_block;
    size_t          used_count;
} array_list_block_info;

typedef struct array_list
{
    size_t          element_size;
    memory_pool*    block_pool;
    size_t          size;
    size_t          elment_count_each_block;
    void*           first_block;
    void*           last_block;
} array_list;

/** Initialize an array_list instance
 *
 *  @param list:            The list instance.
 *  @param element_size:    The size of each element.
 *  @param block_pool:      The memory pool.
 *  @return:                Returns TRUE if success.
 */
boolean array_list_initialize( array_list* list, size_t element_size, memory_pool* block_pool );

/** Destroy the givent array_list instance
 *
 *  @param list: The list instance to be destroy.
 */
void array_list_destroy( array_list* list );

/** Add a new element to the tail of the list.
 *
 *  @param list:        The list instance to add.
 *  @return:            Returns the pointer of new element.
 *                      Returns NULL if failed.
 */
void* array_list_push( array_list* list );

/** Remove the last element from the list.
 *
 *  @param list:        The list instance to remove from.
 *  @return:            None.
 */
void array_list_pop( array_list* list );

/** Get specific element with index.
 *
 *  @param list:        The list instance.
 *  @param index:       The index.
 *  @return:            Returns the pointer of the element.
 *                      Returns NULL if the index doesn't exist.
 */
void* array_list_get( array_list* list, uint32 index );

/** Clear all the element of a list
 *
 *  @param list:        The list instance.
 *  @return:            None.
 */
void array_list_clear( array_list* list );

/** Get the first element of list
 *
 *  @param list:        The list instance.
 *  @return:            Returns the first element.
 *                      Returns NULL if the list is empty.
 */
void* array_list_first( array_list* list );

/** Get the last element of list
 *
 *  @param list:        The list instance.
 *  @return:            Returns the last element.
 *                      Returns NULL if the list is empty.
 */
void* array_list_last( array_list* list );

typedef struct array_list_iterator
{
    array_list*                 list;
    void*                       block;
    array_list_block_info*      block_info;
    uint32                      current_index;
    uint32                      reserved;
} array_list_iterator;

/** Initialize an iterator instance.
 *
 *  @praram iter:       The iterator instance.
 *  @param list:        The list instance to be iterate.
 *  @return:            None.
 */
void array_list_iterator_start( array_list_iterator* iter, array_list* list );

/** Test if the iterator is valid
 *
 *  @param iter:        The iterator instance.
 *  @return:            Returns TRUE if valid.
 */
boolean array_list_iterator_is_valid( array_list_iterator* iter );

/** Move the iterator to the next element.
 *
 *  @param iter:        The iterator.
 *  @return:            None.
 */
void array_list_iterator_next( array_list_iterator* iter );

#define ARRAY_LIST_SIZE( list ) \
    ((list)->size)

#define ARRAY_LIST_IS_EMPTY( list ) \
    ( (list)->size == 0 )

#define ARRAY_LIST_ITERATOR_START( iter, list ) \
    array_list_iterator_start( &iter, list )

#define ARRAY_LIST_ITERATOR_IS_VALID( iter ) \
    array_list_iterator_is_valid( &iter )

#define ARRAY_LIST_ITERATOR_NEXT( iter ) \
    array_list_iterator_next( &iter )

#define ARRAY_LIST_ITERATOR_GET( iter ) \
    ((byte*)(&iter)->block + (&iter)->list->element_size * (&iter)->current_index)

#endif
