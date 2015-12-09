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

typedef struct synchronize_event_group
{
    memory_pool         mempool;
    object_pool         objpool;
    list                elements;
    HANDLE              mutex;
    HANDLE              event;
} synchronize_event_group;

typedef struct synchronize_event_group_element
{
    synchronize_event_group*    group;
    boolean                     flag;
} synchronize_event_group_element;

mutex_type synchronize_create_mutex()
{
    return CreateMutex( NULL, FALSE, NULL );
}

void synchronize_destroy_mutex( mutex_type _mutex )
{
    CloseHandle( _mutex );
}

void synchronize_lock_mutex( mutex_type _mutex )
{
	WaitForSingleObject( _mutex, INFINITE );
}

boolean synchronize_try_lock_mutex( mutex_type _mutex )
{
    return SUCCEEDED(WaitForSingleObject( _mutex, 0 ));
}

void synchronize_unlock_mutex( mutex_type _mutex )
{
    ReleaseMutex( _mutex );
}

event_type synchronize_create_event()
{
    return CreateEvent( NULL, FALSE, FALSE, NULL );
}

void synchronize_destroy_event( event_type _event )
{
    CloseHandle( _event );
}

void synchronize_wait_for_event( event_type _event )
{
    WaitForSingleObject( _event, INFINITE );
}

void synchronize_set_event( event_type _event )
{
    SetEvent( _event );
}

void synchronize_reset_event( event_type _event )
{
    ResetEvent( _event );
}

event_group_type synchronize_create_event_group()
{
    synchronize_event_group* group = (synchronize_event_group*)malloc( sizeof(synchronize_event_group) );
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
    group->mutex = CreateMutex( NULL, FALSE, NULL );
    if ( NULL == group->mutex )
    {
        list_destroy( &group->elements );
        object_pool_destroy( &group->objpool );
        memory_pool_destroy( &group->mempool );
        free( group );
        return NULL;
    }
    group->event = CreateEvent( NULL, TRUE, FALSE, NULL );
    if ( NULL == group->event )
    {
        CloseHandle( group->mutex );
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
    synchronize_event_group* group = (synchronize_event_group*)_group;
    CloseHandle( group->event );
    CloseHandle( group->mutex );
    list_destroy( &group->elements );
    object_pool_destroy( &group->objpool );
    memory_pool_destroy( &group->mempool );
    free( group );
}

event_group_element_type synchronize_event_group_add_element( event_group_type _group )
{
    synchronize_event_group* group = (synchronize_event_group*)_group;
    synchronize_event_group_element* element = NULL;
    WaitForSingleObject( group->mutex, INFINITE );
    element = object_pool_alloc( &group->objpool );
    if ( !list_push_back( &group->elements, &element ) )
    {
        object_pool_free( &group->objpool, element );
        ReleaseMutex( group->mutex );
        return FALSE;
    }
    element->group = group;
    element->flag = FALSE;
    ReleaseMutex( group->mutex );
    return element;
}

void synchronize_event_group_remove_element( event_group_type _group, event_group_element_type _element )
{
    synchronize_event_group* group = (synchronize_event_group*)_group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    WaitForSingleObject( group->mutex, INFINITE );
    LIST_ITERATOR_START( iter, &group->elements );
    while ( LIST_ITERATOR_IS_VALID(iter) )
    {
        element = *(synchronize_event_group_element**)LIST_ITERATOR_GET(iter);
        if ( element == _element )
        {
            list_remove( &group->elements, iter );
            object_pool_free( &group->objpool, element );
            break;
        }
        LIST_ITERATOR_NEXT(iter);
    }
    ReleaseMutex( group->mutex );
    SetEvent( group->event );
}

event_group_element_type synchronize_event_group_wait_for_any( event_group_type _group )
{
    synchronize_event_group* group = (synchronize_event_group*)_group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    synchronize_event_group_element* active_element = NULL;
	WaitForSingleObject( group->mutex, INFINITE );
    do
    {
        if ( LIST_IS_EMPTY( &group->elements ) )
        {
            ReleaseMutex( group->mutex );
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
            ReleaseMutex( group->mutex );
            WaitForSingleObject( group->event, INFINITE );
            ResetEvent( group->event );
            WaitForSingleObject( group->mutex, INFINITE );
        }
    } while ( NULL == active_element );
    active_element->flag = FALSE;
    ReleaseMutex( group->mutex );
    return active_element;
}

event_group_element_type synchronize_event_group_try_wait_for_any( event_group_type _group )
{
    synchronize_event_group* group = (synchronize_event_group*)_group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    synchronize_event_group_element* active_element = NULL;
    WaitForSingleObject( group->mutex, INFINITE );
    if ( LIST_IS_EMPTY( &group->elements ) )
    {
        ReleaseMutex( group->mutex );
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
    ReleaseMutex( group->mutex );
    return active_element;
}

void synchronize_event_group_wait_for_all( event_group_type _group )
{
    synchronize_event_group* group = (synchronize_event_group*)_group;
    list_iterator iter;
    synchronize_event_group_element* element = NULL;
    WaitForSingleObject( group->mutex, INFINITE );
    do
    {
        if ( LIST_IS_EMPTY( &group->elements ) )
        {
            ReleaseMutex( group->mutex );
            return;
        }
        LIST_ITERATOR_START( iter, &group->elements );
        while ( LIST_ITERATOR_IS_VALID(iter) )
        {
            element = *(synchronize_event_group_element**)LIST_ITERATOR_GET(iter);
            if ( !element->flag )
            {
                ReleaseMutex( group->mutex );
                WaitForSingleObject( group->event, INFINITE );
                ResetEvent( group->event );
                WaitForSingleObject( group->mutex, INFINITE );
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
    ReleaseMutex( group->mutex );
}

void synchronize_event_group_wait_for_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = (synchronize_event_group_element*)_element;
    synchronize_event_group* group = element->group;
    WaitForSingleObject( group->mutex, INFINITE );
    while ( !element->flag )
    {
        ReleaseMutex( group->mutex );
        WaitForSingleObject( group->event, INFINITE );
        ResetEvent( group->event );
        WaitForSingleObject( group->mutex, INFINITE );
    }
    element->flag = FALSE;
    ReleaseMutex( group->mutex );
}

boolean synchronize_event_group_try_wait_for_element( event_group_element_type _element )
{
    boolean ret = FALSE;
    synchronize_event_group_element* element = (synchronize_event_group_element*)_element;
    synchronize_event_group* group = element->group;
    WaitForSingleObject( group->mutex, INFINITE );
    ret = element->flag;
    element->flag = FALSE;
    ReleaseMutex( group->mutex );
    return ret;
}

void synchronize_event_group_set_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = (synchronize_event_group_element*)_element;
    synchronize_event_group* group = element->group;
    WaitForSingleObject( group->mutex, INFINITE );
    element->flag = TRUE;
    ReleaseMutex( group->mutex );
    SetEvent( group->event );
}

void synchronize_event_group_reset_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = (synchronize_event_group_element*)_element;
    synchronize_event_group* group = element->group;
    WaitForSingleObject( group->mutex, INFINITE );
    element->flag = FALSE;
    ReleaseMutex( group->mutex );
}

event_group_type synchronize_event_group_get_group_by_element( event_group_element_type _element )
{
    synchronize_event_group_element* element = (synchronize_event_group_element*)_element;
    return element->group;
}

