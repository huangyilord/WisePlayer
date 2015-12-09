#if !defined ( _GRAPH_EXPLORER_H_ )
#define _GRAPH_EXPLORER_H_

#include "graph.h"
#include "explore_node_map.h"
#include "red_black_tree.h"

typedef struct graph_explorer graph_explorer;

typedef red_black_tree_node_compare graph_explorer_node_compare_function;
typedef hash_table_key_compare_function graph_explorer_node_key_equal_function;
typedef hash_table_hash_code_function graph_explorer_node_key_hash_function;

typedef struct graph_explorer_callbacks
{
    graph_explorer_node_compare_function        compare_function;
    graph_explorer_node_key_equal_function      key_equal_function;
    graph_explorer_node_key_hash_function       key_hash_function;
} graph_explorer_callbacks;

struct graph_explorer
{
    memory_pool*                            mempool;
    graph*                                  current_graph;
    explore_node_map                        explored_nodes;
    red_black_tree                          opening_nodes;
    size_t                                  key_size;
    size_t                                  node_size;
    void*                                   graph_context;
};

boolean graph_explorer_initialize( graph_explorer* explorer, size_t node_key_size, size_t node_info_size, memory_pool* pool );

void graph_explorer_destroy( graph_explorer* explorer );

boolean graph_explorer_start_explore( graph_explorer* explorer, graph* g, const void* node, graph_explorer_callbacks callbacks );

boolean graph_explorer_step( graph_explorer* explorer );

void graph_explorer_finish_explore( graph_explorer* explorer );

#endif /* defined(_GRAPH_EXPLORER_H_) */
