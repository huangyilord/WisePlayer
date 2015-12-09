//
//  synchronize.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "synchronize.h"

#include "list.h"
#include "common_debug.h"

typedef struct synchronize_event
{
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
    boolean             flag;
} synchronize_event;

typedef struct synchronize_event_group
{
    memory_pool         mempool;
    object_pool         objpool;
    list                elements;
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
} synchronize_event_group;

typedef struct synchronize_event_group_element
{
    synchronize_event_group*    group;
    boolean                     flag;
} synchronize_event_group_element;

mutex_type synchronize_create_mutex()
{
    pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
    if ( NULL == mutex )
    {
        return NULL;
    }
    if ( pthread_mutex_init( mutex, NULL) != 0 )
    {
        free( mutex );
        return NULL;
    }
    return mutex;
}

void synchronize_destroy_mutex( mutex_type _mutex )
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)_mutex;
    pthread_mutex_destroy( mutex );
    free( mutex );
}

void synchronize_lock_mutex( mutex_type _mutex )
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)_mutex;
    pthread_mutex_lock( mutex );
}

boolean synchronize_try_lock_mutex( mutex_type _mutex )
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)_mutex;
    return pthread_mutex_trylock( mutex ) == 0;
}

void synchronize_unlock_mutex( mutex_type _mutex )
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)_mutex;
    pthread_mutex_unlock( mutex );
}

event_type synchronize_create_event()
{
    synchronize_event* event = malloc( sizeof(synchronize_event) );
    if ( NULL == event )
    {
        return NULL;
    }
    memset( event, 0, sizeof(synchronize_event) );
    if (pthread_mutex_init(&(event->mutex), NULL) != 0)
    {
        free( event );
        return NULL;
    }
    if (pthread_cond_init(&(event->cond), NULL) != 0)
    {
        pthread_mutex_destroy(&(event->mutex));
        free( event );
        return NULL;
    }
    event->flag = FALSE;
    return event;
}

void synchronize_destroy_event( event_type _event )
{
    synchronize_event* event = _event;
    pthread_mutex_destroy(&(event->mutex));
    pthread_cond_destroy(&(event->cond));
}

void synchronize_wait_for_event( event_type _event )
{
    synchronize_event* event = _event;
    pthread_mutex_lock(&(event->mutex));
    while (!event->flag)
    {
        pthread_cond_wait(&(event->cond), &(event->mutex));
    }
    event->flag = FALSE;
    pthread_mutex_unlock(&(event->mutex));
}

void synchronize_set_event( event_type _event )
{
    synchronize_event* event = _event;
    pthread_mutex_lock(&(event->mutex));
    event->flag = TRUE;
    pthread_cond_signal(&(event->cond));
    pthread_mutex_unlock(&(event->mutex));
}

void synchronize_reset_event( event_type _event )
{
    synchronize_event* event = _event;
    pthread_mutex_lock(&(event->mutex));
    event->flag = FALSE;
    pthread_mutex_unlock(&(event->mutex));
}

event_group_type synchronize_create_event_group()
{
    synchronize_event_group* group = malloc( sizeof(synchronize_event_group) );
    if ( NULL == group )
    {
        return NULL;
    }
    memset( group, 0, sizeof(synchronize_event_group) );
    memory_pool_initialize( &group->mempool, 1024, 1 );
    if ( !object_pool_initialize( &group->objpool, &group->mempool, sizeof(synchronize_event_group_element) ) )
    {
        memory_pool_destroy( &group->mempool );
        free( group );
        return NULL;
    }
    if ( !list_initialize( &group->elements, sizeof(synchronize_event_group_element*), &group->mempool ) )
    {
        object_pool_destroy( &group->objpool );
        memory_pool_destroy( &group->mempool );
        free( group );
        return NULL;
    }
    if (pthread_mutex_init(&(group->mutex), NULL) != 0)
    {
        list_destroy( &group->elements );
        object_pool_destroy( &group->objpool );
        memory_pool_destroy( &group->mempool );
        free( group );
        return NULL;
    }
    if (pthread_cond_init(&(group->cond), NULL) != 0)
    {
        pthread_mutex_destroy( &(group->mutex) );
        list_destroy( &group->elements );
        object_pool_destroy( &group->objpool );
        memory_pool_destroy( &group->mempool );
        free( group );
        return NULL;
    }

    return group;
}

void synchronize_destroy_event_group( event_group_type _group )
{
    synchronize_event_group* group = _group;
    pthread_cond_destroy( &(group->cond) );
    pthread_mutex_destroy( &(group->mutex) );
    list_destroy( &group->elements );
    object_pool_destroy( &group->objpool );
    memory_pool_destroy( &group->mempool );
    free( group );
}

event_group_element_type synchronize_event_group_add_element( event_group_type _group )
{
    synchronize_event_group* group = _group;
    synchronize_event_group_element* element = NULL;
    pthread_mutex_lock( &(group->mutex) );
    element = object_pool_alloc( &group->objpool );
    if ( !list_push_back( &group->elements, &element ) )
    {
        object_pool_free( &group->objpool, element );
        pthread_mutex_unlock( &(group->mutex) );
        return FALSE;
    }
    element->group = group;
    element->flag = FALSE;
    pthread_mutex_unlock( &(group->mutex) );
    return element;
}

void synchronize_event_group_remove_element( event_group_type _group, event_group_element_type _element )
{
    synchronize_event_group* group = _group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    pthread_mutex_lock( &(group->mutex) );
    LIST_ITERATOR_START( iter, &group->elements );
    while ( LIST_ITERATOR_IS_VALID(iter) )
    {
        element = *(synchronize_event_group_element**)LIST_ITERATOR_GET(iter);
        if ( element == _element )
        {
            list_remove( &group->elements, iter );
            object_pool_free( &group->objpool, element );
            pthread_cond_broadcast( &(group->cond) );
            break;
        }
        LIST_ITERATOR_NEXT(iter);
    }
    pthread_mutex_unlock( &(group->mutex) );
}

event_group_element_type synchronize_event_group_wait_for_any( event_group_type _group )
{
    synchronize_event_group* group = _group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    synchronize_event_group_element* active_element = NULL;
    pthread_mutex_lock( &(group->mutex) );
    do
    {
        if ( LIST_IS_EMPTY( &group->elements ) )
        {
            pthread_mutex_unlock( &(group->mutex) );
            return NULL;
        }
        LIST_ITERATOR_START( iter, &group->elements );
        while ( LIST_ITERATOR_IS_VALID(iter) )
        {
            element = *(synchronize_event_group_element**)LIST_ITERATOR_GET(iter);
            if ( element->flag )
            {
                active_element = element;
                break;
            }
            LIST_ITERATOR_NEXT(iter);
        }
        if ( NULL == active_element )
        {
            pthread_cond_wait( &(group->cond), &(group->mutex) );
        }
    } while ( NULL == active_element );
    active_element->flag = FALSE;
    pthread_mutex_unlock( &(group->mutex) );
    return active_element;
}

event_group_element_type synchronize_event_group_try_wait_for_any( event_group_type _group )
{
    synchronize_event_group* group = _group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    synchronize_event_group_element* active_element = NULL;
    pthread_mutex_lock( &(group->mutex) );
    if ( LIST_IS_EMPTY( &group->elements ) )
    {
        pthread_mutex_unlock( &(group->mutex) );
        return NULL;
    }
    LIST_ITERATOR_START( iter, &group->elements );
    while ( LIST_ITERATOR_IS_VALID(iter) )
    {
        element = *(synchronize_event_group_element**)LIST_ITERATOR_GET(iter);
        if ( element->flag )
        {
            active_element = element;
            active_element->flag = FALSE;
            break;
        }
        LIST_ITERATOR_NEXT(iter);
    }
    pthread_mutex_unlock( &(group->mutex) );
    return active_element;
}

void synchronize_event_group_wait_for_all( event_group_type _group )
{
    synchronize_event_group* group = _group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    pthread_mutex_lock( &(group->mutex) );
    do
    {
        if ( LIST_IS_EMPTY( &group->elements ) )
        {
            pthread_mutex_unlock( &(group->mutex) );
            return;
        }
        LIST_ITERATOR_START( iter, &group->elements );
        while ( LIST_ITERATOR_IS_VALID(iter) )
        {
            element = *(synchronize_event_group_element**)LIST_ITERATOR_GET(iter);
            if ( !element->flag )
            {
                pthread_cond_wait( &(group->cond), &(group->mutex) );
                continue;
            }
            LIST_ITERATOR_NEXT(iter);
        }
        LIST_ITERATOR_START( iter, &group->elements );
        while ( LIST_ITERATOR_IS_VALID(iter) )
        {
            element = *(synchronize_event_group_element**)LIST_ITERATOR_GET(iter);
            element->flag = FALSE;
            LIST_ITERATOR_NEXT(iter);
        }
        break;
    } while ( TRUE );
    pthread_mutex_unlock( &(group->mutex) );
}

void synchronize_event_group_wait_for_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = _element;
    synchronize_event_group* group = element->group;
    pthread_mutex_lock( &(group->mutex) );
    while ( !element->flag )
    {
        pthread_cond_wait( &(group->cond), &(group->mutex) );
    }
    element->flag = FALSE;
    pthread_mutex_unlock( &(group->mutex) );
}

boolean synchronize_event_group_try_wait_for_element( event_group_element_type _element )
{
    boolean ret = FALSE;
    synchronize_event_group_element* element = _element;
    synchronize_event_group* group = element->group;
    pthread_mutex_lock( &(group->mutex) );
    ret = element->flag;
    element->flag = FALSE;
    pthread_mutex_unlock( &(group->mutex) );
    return ret;
}

void synchronize_event_group_set_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = _element;
    synchronize_event_group* group = element->group;
    pthread_mutex_lock( &(group->mutex) );
    element->flag = TRUE;
    pthread_cond_broadcast( &(group->cond) );
    pthread_mutex_unlock( &(group->mutex) );
}

void synchronize_event_group_reset_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = _element;
    synchronize_event_group* group = element->group;
    pthread_mutex_lock( &(group->mutex) );
    element->flag = FALSE;
    pthread_mutex_unlock( &(group->mutex) );
}

event_group_type synchronize_event_group_get_group_by_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = _element;
    return element->group;
}

