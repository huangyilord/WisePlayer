//
//  hash_set.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "hash_set.h"

boolean hash_set_initialize( hash_set* set, size_t value_size, memory_pool* pool )
{
    if ( !list_initialize( &set->keys, value_size, pool ) )
    {
        return FALSE;
    }
    if ( !hash_table_initialize( &set->values, value_size, sizeof(list_iterator), pool ) )
    {
        list_destroy( &set->keys );
        return FALSE;
    }
    set->value_size = value_size;
    return TRUE;
}

void hash_set_destroy( hash_set* set )
{
    hash_table_destroy( &set->values );
    list_destroy( &set->keys );
    set->value_size = 0;
}

boolean hash_set_exist( hash_set* set, const void* value )
{
    return hash_table_exist( &set->values, value );
}

boolean hash_set_insert( hash_set* set, const void* value )
{
    if ( hash_table_exist( &set->values, value ) )
    {
        return TRUE;
    }
    if ( !list_push_back( &set->keys, value ) )
    {
        return FALSE;
    }
    if ( !hash_table_insert( &set->values, value, &set->keys.last ) )
    {
        list_pop_back( &set->keys );
        return FALSE;
    }
    return TRUE;
}

void hash_set_remove( hash_set* set, const void* value )
{
    list_iterator* iter = (list_iterator*)hash_table_get( &set->values, value );
    if ( NULL != iter )
    {
        list_remove( &set->keys, *iter );
        hash_table_remove( &set->values, value );
    }
}

void hash_set_clear( hash_set* set )
{
    list_clear( &set->keys );
    hash_table_clear( &set->values );
}

boolean hash_set_equal( hash_set* set1, hash_set* set2 )
{
    hash_set_iterator iter;
    if ( set1->keys.size != set2->keys.size || set1->value_size != set2->value_size )
    {
        return FALSE;
    }
    if ( 0 == set1->value_size )
    {
        return TRUE;
    }
    HASH_SET_ITERATOR_START( iter, set1 );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        if ( !hash_set_exist( set2, HASH_SET_ITERATOR_GET( iter ) ) )
        {
            return FALSE;
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    HASH_SET_ITERATOR_START( iter, set2 );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        if ( !hash_set_exist( set1, HASH_SET_ITERATOR_GET( iter ) ) )
        {
            return FALSE;
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    return TRUE;
}

boolean hash_set_copy( hash_set* set_dst, hash_set* set_src )
{
    hash_set_iterator iter;
    hash_set_clear( set_dst );
    HASH_SET_ITERATOR_START( iter, set_src );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        if ( !hash_set_insert( set_dst, HASH_SET_ITERATOR_GET( iter ) ) )
        {
            hash_set_clear( set_dst );
            return FALSE;
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    return TRUE;
}

boolean hash_set_sub( hash_set* result, hash_set* set1, hash_set* set2 )
{
    hash_set_iterator iter;
    HASH_SET_ITERATOR_START(iter, set1);
    while ( HASH_SET_ITERATOR_IS_VALID(iter) )
    {
        if ( !hash_set_insert( result, HASH_SET_ITERATOR_GET(iter) ) )
        {
            hash_set_clear( result );
            return FALSE;
        }
        HASH_SET_ITERATOR_NEXT(iter);
    }
    HASH_SET_ITERATOR_START(iter, set2);
    while ( HASH_SET_ITERATOR_IS_VALID(iter) )
    {
        hash_set_remove( result, HASH_SET_ITERATOR_GET(iter) );
        HASH_SET_ITERATOR_NEXT(iter);
    }
    return TRUE;
}

boolean hash_set_add( hash_set* result, hash_set* set1, hash_set* set2 )
{
    hash_set_iterator iter;
    HASH_SET_ITERATOR_START(iter, set1);
    while ( HASH_SET_ITERATOR_IS_VALID(iter) )
    {
        if ( !hash_set_insert( result, HASH_SET_ITERATOR_GET(iter) ) )
        {
            hash_set_clear( result );
            return FALSE;
        }
        HASH_SET_ITERATOR_NEXT(iter);
    }
    HASH_SET_ITERATOR_START(iter, set2);
    while ( HASH_SET_ITERATOR_IS_VALID(iter) )
    {
        if ( !hash_set_insert( result, HASH_SET_ITERATOR_GET(iter) ) )
        {
            hash_set_clear( result );
            return FALSE;
        }
        HASH_SET_ITERATOR_NEXT(iter);
    }
    return TRUE;
}

boolean hash_set_intersect( hash_set* result, hash_set* set1, hash_set* set2 )
{
    hash_set_iterator iter;
    HASH_SET_ITERATOR_START(iter, set1);
    while ( HASH_SET_ITERATOR_IS_VALID(iter) )
    {
        if ( hash_set_exist( set2, HASH_SET_ITERATOR_GET(iter) ))
        {
            if ( !hash_set_insert( result, HASH_SET_ITERATOR_GET(iter) ) )
            {
                hash_set_clear( result );
                return FALSE;
            }
        }
        HASH_SET_ITERATOR_NEXT(iter);
    }
    return TRUE;
}

