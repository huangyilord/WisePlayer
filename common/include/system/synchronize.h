//
//  synchronize.h
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _SYNCHRONIZE_H_ )
#define _SYNCHRONIZE_H_

#include "common_typedefs.h"

/**
 *  Create a mutex
 *
 *  @return:            Returns the new created mutex. Return NULL if failed.
 */
mutex_type synchronize_create_mutex();

/**
 *  Destroy a mutex
 *
 *  @param mutex:       The mutex to destroy.
 */
void synchronize_destroy_mutex( mutex_type mutex );

/**
 *  Lock a mutex
 *
 *  @param mutex:       The mutex to lock.
 */
void synchronize_lock_mutex( mutex_type mutex );

/**
 *  Try to lock a mutex
 *  If failed, it will return immediately with FALSE.
 *
 *  @param mutex:       The mutex to lock.
 *  @return:            Returns TRUE if success.
 */
boolean synchronize_try_lock_mutex( mutex_type mutex );

/**
 *  Unlock a locked mutex
 *
 *  @param mutex:       The mutex to unlock.
 */
void synchronize_unlock_mutex( mutex_type mutex );

/**
 *  Create a event
 *  The new created event is initialized to be unset.
 *
 *  @return:            Returns the new created event. Retruns NULL if failed.
 */
event_type synchronize_create_event();

/**
 *  Destroy the event created by thread_create_event.
 *
 *  @param event:       The event to be destroy.
 */
void synchronize_destroy_event( event_type event );

/**
 *  Wait infinite for a specific event.
 *
 *  @param event:       The event handle.
 */
void synchronize_wait_for_event( event_type event );

/**
 *  Trigger an event
 *  When trigger an event, only one 'thread_wait_for_event' will return successfully.
 *
 *  @param event:       The event handle.
 */
void synchronize_set_event( event_type event );

/**
 *  Reset an event
 *
 *  @param event:       The event handle.
 */
void synchronize_reset_event( event_type event );

/**
 *  Create an event group
 *
 *  @return:            Returns the created event group.
 */
event_group_type synchronize_create_event_group();

/**
 *  Destroy an event group
 *
 *  @param group:       The event group to destroy.
 */
void synchronize_destroy_event_group( event_group_type group );

/**
 *  Add an event to a group.
 *
 *  @param group:       The event group.
 *  @return:            Returns the new element. If failed, the return value would be NULL.
 */
event_group_element_type synchronize_event_group_add_element( event_group_type group );

/**
 *  Remove an event from a group.
 *
 *  @param group:       The event group.
 *  @param element:     The element to remove which returned by synchronize_event_group_add_element
 */
void synchronize_event_group_remove_element( event_group_type group, event_group_element_type element );

/**
 *  Wait an event of a group.
 *
 *  @param group:       The event group.
 *  @return:            Returns the active element.
 *                      If more than one elements are active, only one of them will be returned.
 *                      And the status of that element would be automatically reset.
 *                      If the group is empty, this function will return immediately with NULL.
 */
event_group_element_type synchronize_event_group_wait_for_any( event_group_type group );

/**
 *  Same as synchronize_event_group_wait_for_any.
 *  This function will return immediately if none element is availible.
 *
 *  @param group:       The event group.
 *  @return:            Returns the active element.
 *                      If more than one elements are active, only one of them will be returned.
 *                      And the status of that element would be automatically reset.
 *                      If the group is empty, this function will return immediately with NULL.
 */
event_group_element_type synchronize_event_group_try_wait_for_any( event_group_type group );

/**
 *  Wait for all events in a group.
 *
 *  @param group:       The event group.
 */
void synchronize_event_group_wait_for_all( event_group_type group );

/**
 *  Wait for an specific element in a group.
 *
 *  @param element:     The element to wait.
 */
void synchronize_event_group_wait_for_element( event_group_element_type element );

/**
 *  Same as synchronize_event_group_wait_for_element.
 *  This function will return immediately if the element is unavailible.
 *
 *  @param element:     The element to wait.
 *  @return:            Returns FALSE if the element is unavailible.
 */
boolean synchronize_event_group_try_wait_for_element( event_group_element_type element );

/**
 *  Set an event in group
 *
 *  @param element:     The element to set.
 */
void synchronize_event_group_set_element( event_group_element_type element );

/**
 *  Reset an event in group
 *
 *  @param element:     The element to reset.
 */
void synchronize_event_group_reset_element( event_group_element_type element );

/**
 *  Get the group by element.
 *
 *  @param element:     The event element.
 *  @return:            Returns the group.
 */
event_group_type synchronize_event_group_get_group_by_element( event_group_element_type element );

#endif // #if !defined ( _SYNCHRONIZE_H_ )
