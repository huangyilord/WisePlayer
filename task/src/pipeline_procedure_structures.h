#if !defined ( _PIPELINE_PROCEDURE_STRUCTURES_H_ )
#define _PIPELINE_PROCEDURE_STRUCTURES_H_

#include "list.h"

typedef enum exchange_data_type
{
    exchange_data_custom = 0
    , exchange_data_shutdown
} exchange_data_type;

typedef struct exchange_data_desc
{
    size_t              size;
    void*               data;
    exchange_data_type  type;
} exchange_data_desc;

typedef struct pipeline_procedure pipeline_procedure;
typedef struct pipeline_procedure_input pipeline_procedure_input;
typedef struct pipeline_procedure_output pipeline_procedure_output;

typedef struct pipeline_connection
{
    pipeline_procedure_input*       target;
    pipeline_procedure_output*      source;
    list                            exchange_data;
} pipeline_connection;

/**
 *  Initialize a pipeline connection instance.
 *
 *  @param connection:      The connection instance to initialize.
 *  @param source:          Data source
 *  @param target:          Target procedure input
 *  @param max_data_num:    The max blocked data package.
 *  @param pool:            The memory pool.
 *  @return:                Returns TRUE if succeeded.
 */
boolean pipeline_connection_initialize( pipeline_connection* connection, uint32 max_data_num, memory_pool* pool );

/**
 *  Destroy a pipeline connection instance.
 *
 *  @param connection:      The connection instance.
 */
void pipeline_connection_destroy( pipeline_connection* connection );

/**
 *  Connect an input with an output.
 *
 *  @param connection:      The connection instance.
 *  @param output:          The output instance.
 *  @param input:           The input instance.
 *  @return:                Returns TRUE if success.
 */
boolean pipeline_connection_connect( pipeline_connection* connection, pipeline_procedure_output* output, pipeline_procedure_input* input );

/**
 *  Disconnect a connection.
 *
 *  @param connection:      The connection instance.
 */
void pipeline_connection_disconnect( pipeline_connection* connection );


typedef struct pipeline_procedure_input
{
    list* volatile              incoming_task;
    list                        task_list;
    pipeline_connection*        connection;
    pipeline_procedure*         procedure;
    event_group_element_type    new_input_event;
} pipeline_procedure_input;

/**
 *  Initialize a pipeline_procedure_input instance.
 *
 *  @param input:           The instance to initialize.
 *  @return:                Returns TRUE if success.
 */
boolean pipeline_procedure_input_initialize( pipeline_procedure_input* input, pipeline_procedure* procedure, memory_pool* pool );

/**
 *  Destroy a pipeline_procedure_input instance.
 *
 *  @param input:           The instance to destroy.
 */
void pipeline_procedure_input_destroy( pipeline_procedure_input* input );

/**
 *  Connect an input with a pipeline_connection
 *
 *  @param input:           The input instance
 *  @param connection:      The connection instance
 *  @return:                Returns TRUE if success.
 */
boolean pipeline_procedure_input_connect( pipeline_procedure_input* input, pipeline_connection* connection );

/**
 *  Disconnect an input
 *
 *  @param input:           The input instance.
 */
void pipeline_procedure_input_disconnect( pipeline_procedure_input* input );

/**
 *  To check whether an input has data.
 *  This function will return immediately.
 *
 *  @param input:           The input instance.
 *  @return:                Returns TRUE if there is input data.
 */
boolean pipeline_procedure_input_has_data( pipeline_procedure_input* input );

/**
 *  Try to get input data.
 *  This function will return immediately.
 *
 *  @param input:           The input instance.
 *  @return:                Returns the pointer of input data.
 *                          Returns NULL if no input.
 */
exchange_data_desc* pipeline_procedure_input_try_get_data( pipeline_procedure_input* input );

/**
 *  Wait to get input data.
 *  This function won't return until there is input.
 *
 *  @param input:           The input instance.
 *  @return:                Returns the pointer of input data.
 */
exchange_data_desc* pipeline_procedure_input_get_data( pipeline_procedure_input* input );

/**
 *  Reset input
 *
 *  @param input:           The input instance.
 */
void pipeline_procedure_input_reset( pipeline_procedure_input* input );

/**
 *  Move on to next input data.
 *
 *  @param input:           The input instance.
 */
void pipeline_procedure_input_moveon( pipeline_procedure_input* input );

/**
 *  Send a shut down command to the intput.
 *
 *  @param intput:          The input instance.
 */
void pipeline_procedure_input_shutdown( pipeline_procedure_input* input );

typedef struct pipeline_procedure_output
{
    size_t                      data_size;
    list* volatile              outgoing_task;
    list                        task_list;
    pipeline_connection*        connection;
    pipeline_procedure*         procedure;
    event_type                  recycle_data_event;
} pipeline_procedure_output;

/**
 *  Initialize a pipeline_procedure_output instance.
 *
 *  @param output:          The instance to initialize.
 *  @return:                Returns TRUE if success.
 */
boolean pipeline_procedure_output_initialize( pipeline_procedure_output* output, pipeline_procedure* procedure, memory_pool* pool );

/**
 *  Reset exchange data size
 *
 *  @param output:          The instance to initialize.
 *  @param data_size:       New data size.
 */
void pipeline_procedure_output_set_data_size( pipeline_procedure_output* output, size_t data_size );

/**
 *  Destroy a pipeline_procedure_output instance.
 *
 *  @param output:          The instance to destroy.
 */
void pipeline_procedure_output_destroy( pipeline_procedure_output* output );

/**
 *  Connect an output with a pipeline_connection
 *
 *  @param output:          The output instance
 *  @param connection:      The connection instance
 *  @return:                Returns TRUE if success.
 */
boolean pipeline_procedure_output_connect( pipeline_procedure_output* output, pipeline_connection* connection );

/**
 *  Disconnect an output
 *
 *  @param output:          The output instance.
 */
void pipeline_procedure_output_disconnect( pipeline_procedure_output* output );

/**
 *  To check whether an output has data.
 *  This function will return immediately.
 *
 *  @param output:          The output instance.
 *  @return:                Returns TRUE if there is output data.
 */
boolean pipeline_procedure_output_has_data( pipeline_procedure_output* output );

/**
 *  Try to get output data.
 *  This function will return immediately.
 *
 *  @param output:          The output instance.
 *  @return:                Returns the pointer of output data.
 *                          Returns NULL if no output.
 */
exchange_data_desc* pipeline_procedure_output_try_get_data( pipeline_procedure_output* output );

/**
 *  Wait to get output data.
 *  This function won't return until there is output.
 *
 *  @param output:          The output instance.
 *  @return:                Returns the pointer of output data.
 */
exchange_data_desc* pipeline_procedure_output_get_data( pipeline_procedure_output* output );

/**
 *  Reset output
 *
 *  @param output:          The output instance.
 */
void pipeline_procedure_output_reset( pipeline_procedure_output* output );

/**
 *  Output data to the connected input
 *
 *  @param output:          The output instance.
 */
void pipeline_procedure_output_passon( pipeline_procedure_output* output );

/**
 *  Send a shut down command to the output.
 *
 *  @param output:          The output instance.
 */
void pipeline_procedure_output_shutdown( pipeline_procedure_output* output );

#endif /* defined(_PIPELINE_PROCEDURE_STRUCTURES_H_) */
