//
//  thread.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _THREAD_H_ )
#define _THREAD_H_

#include "common_typedefs.h"

typedef uint32 (*thread_function)( void* user_data );

/**
 *  Create a thread
 *
 *  @param thread:      Output thread instance.
 *  @param func:        The thread function.
 *  @param user_data:   The thread context.
 */
boolean thread_create_thread( thread_type* thread, thread_function func, void* user_data );

/**
 *  Wait for the specific thread to finish.
 *  If the thread doesn't exist, return immediately.
 *
 *  @param thread:      The thread to wait for.
 */
void thread_wait_for_thread( thread_type* thread );

/**
 *  Get current thread
 *
 *  @param thread:      ouput the thread
 */
void thread_get_current_thread( thread_type* thread );

/**
 *  To check whether a thread equals another.
 *
 *  @param thread1:     thread 1.
 *  @param thread2:     thread 2.
 *  @return:            Returns TRUE if the two threads are the same.
 */
boolean thread_equal( const thread_type* thread1, const thread_type* thread2 );

/**
 *  Sleep for some time
 *
 *  @param milliseconds: How many miliseconds to sleep.
 */
void thread_sleep( uint32 milliseconds );

/**
 *  Relinquish the CPU
 *
 */
void thread_yield();



#endif
