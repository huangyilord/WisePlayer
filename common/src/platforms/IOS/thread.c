//
//  thread.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "thread.h"
#include <unistd.h>

boolean thread_create_thread( thread_type* thread, thread_function func, void* user_data )
{
    if ( 0 != pthread_create( thread, NULL, (void *(*)(void *))func, user_data ) )
    {
        return FALSE;
    }
    return TRUE;
}

void thread_wait_for_thread( thread_type* thread )
{
    pthread_join( *thread, NULL );
}

void thread_get_current_thread( thread_type* thread )
{
    *thread = pthread_self();
}

boolean thread_equal( const thread_type* thread1, const thread_type* thread2 )
{
    return pthread_equal( *thread1, *thread2 );
}

void thread_sleep( uint32 milliseconds )
{
    usleep( milliseconds * 1000 );
}

void thread_yield()
{
    pthread_yield_np();
}
