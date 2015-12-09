//
//  list.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "list.h"
#include "common_debug.h"

boolean list_initialize( list* plist, size_t element_size, memory_pool* pool )
{
    if ( !object_pool_initialize( &plist->node_pool, pool, sizeof(list_node) + element_size ) )
    {
        return FALSE;
    }
    plist->first = NULL;
    plist->last = NULL;
    plist->size = 0;
    plist->element_size = element_size;
    return TRUE;
}

void list_destroy( list* plist )
{
    list_clear( plist );
    object_pool_destroy( &plist->node_pool );
}

boolean list_push_front( list* plist, const void* value )
{
    list_node* pnode = object_pool_alloc( &plist->node_pool );
    if ( NULL == pnode )
    {
        return FALSE;
    }
    pnode->next = plist->first;
    pnode->prev = NULL;
    plist->first = pnode;
    if ( NULL != pnode->next )
    {
        ASSERT( NULL == pnode->next->prev );
        ASSERT( 0 < plist->size );
        pnode->next->prev = pnode;
    }
    else
    {
        ASSERT( NULL == plist->last );
        ASSERT( 0 == plist->size );
        plist->last = pnode;
    }
    memcpy( pnode + 1, value, plist->element_size );
    ++ plist->size;
    return TRUE;
}

void list_pop_front( list* plist )
{
    list_node* pnode = plist->first;
    if ( NULL != pnode )
    {
        ASSERT( 0 < plist->size );
        -- plist->size;
        plist->first = pnode->next;
        if ( NULL != pnode->next )
        {
            ASSERT( 0 < plist->size );
            pnode->next->prev = NULL;
        }
        else
        {
            ASSERT( 0 == plist->size );
            ASSERT( plist->last == pnode );
            plist->last = NULL;
        }
        object_pool_free( &plist->node_pool, pnode );
    }
}

void* list_front( list* plist )
{
    if ( NULL != plist->first )
    {
        return plist->first + 1;
    }
    return NULL;
}

boolean list_push_back( list* plist, const void* value )
{
    list_node* pnode = object_pool_alloc( &plist->node_pool );
    if ( NULL == pnode )
    {
        return FALSE;
    }
    pnode->next = NULL;
    pnode->prev = plist->last;
    plist->last = pnode;
    if ( NULL != pnode->prev )
    {
        ASSERT( NULL == pnode->prev->next );
        ASSERT( 0 < plist->size );
        pnode->prev->next = pnode;
    }
    else
    {
        ASSERT( NULL == plist->first );
        ASSERT( 0 == plist->size );
        plist->first = pnode;
    }
    memcpy( pnode + 1, value, plist->element_size );
    ++ plist->size;
    return TRUE;
}

void list_pop_back( list* plist )
{
    list_node* pnode = plist->last;
    if ( NULL != pnode )
    {
        ASSERT( 0 < plist->size );
        -- plist->size;
        plist->last = pnode->prev;
        if ( NULL != pnode->prev )
        {
            ASSERT( 0 < plist->size );
            pnode->prev->next = NULL;
        }
        else
        {
            ASSERT( 0 == plist->size );
            ASSERT( plist->first == pnode );
            plist->first = NULL;
        }
        object_pool_free( &plist->node_pool, pnode );
    }
}

void* list_back( list* plist )
{
    if ( NULL != plist->last )
    {
        return plist->last + 1;
    }
    return NULL;
}

void list_clear( list* plist )
{
    list_node* current = plist->first;
    list_node* next = NULL;
    while ( NULL != current )
    {
        next = current->next;
        object_pool_free( &plist->node_pool, current );
        current = next;
    }
    plist->first = NULL;
    plist->last = NULL;
    plist->size = 0;
}

void list_remove( list* plist, list_iterator iter )
{
    list_node* pnode = iter;
    if ( NULL != pnode->next )
    {
        pnode->next->prev = pnode->prev;
    }
    if ( NULL != pnode->prev )
    {
        pnode->prev->next = pnode->next;
    }
    if ( plist->first == pnode )
    {
        plist->first = pnode->next;
    }
    if ( plist->last == pnode )
    {
        plist->last = pnode->prev;
    }
    object_pool_free( &plist->node_pool, pnode );
    -- plist->size;
}

