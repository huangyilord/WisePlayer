#if !defined ( _PIPE_LINE_H_ )
#define _PIPE_LINE_H_

#include "pipeline_procedure.h"
#include "task_processor_thread_pool.h"
#include "hash_set.h"
#include "hash_map.h"

typedef struct pipeline_procedure_desc pipeline_procedure_desc;
typedef struct pipeline
{
    memory_pool*                    mempool;
    list                            procedures;
    hash_map                        procedure_desc_map;
    array_list                      connections;
    task_processor_thread_pool*     processor_thread_pool;
    hash_set                        src;
} pipeline;

boolean pipeline_initialize( pipeline* p, task_processor_thread_pool* processor_thread_pool, memory_pool* pool );

void pipeline_destroy( pipeline* p );

pipeline_procedure* pipeline_add_src( pipeline* p, pipeline_procedure_desc* desc );

void pipeline_clear( pipeline* p );

void pipeline_stop( pipeline* p );

void pipeline_start( pipeline* p );

#endif /* defined(_PIPE_LINE_H_) */
