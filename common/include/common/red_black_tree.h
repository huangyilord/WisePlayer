//
//  red_black_tree.h
//  common
//
//  Created by Yi Huang on 28/4/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _RED_BLACK_TREE_H_ )
#define _RED_BLACK_TREE_H_

#include "object_pool.h"

typedef struct red_black_tree_node red_black_tree_node;
typedef struct red_black_tree red_black_tree;
typedef int32 (*red_black_tree_node_compare)( red_black_tree* tree, const void* element1, const void* element2 );

typedef enum red_black_tree_node_type
{
    red_black_tree_node_type_red = 0
    , red_black_tree_node_type_black
} red_black_tree_node_type;

struct red_black_tree_node
{
    struct red_black_tree_node*     left;
    struct red_black_tree_node*     right;
    struct red_black_tree_node*     parent;
    red_black_tree_node_type        type;
};

struct red_black_tree
{
    red_black_tree_node*            root;
    size_t                          size;
    size_t                          element_size;
    red_black_tree_node_compare     compare_fun;
    object_pool                     node_pool;
};

typedef red_black_tree_node* red_black_tree_iterator;

boolean red_black_tree_initialize( red_black_tree* tree, size_t element_size, memory_pool* pool );

void red_black_tree_destroy( red_black_tree* tree );

void red_black_tree_clear( red_black_tree* tree );

red_black_tree_iterator red_black_tree_insert( red_black_tree* tree, const void* element );

void red_black_tree_remove( red_black_tree* tree, const void* element );

red_black_tree_iterator red_black_tree_find( red_black_tree* tree, const void* element );

red_black_tree_iterator red_black_tree_lower_bound( red_black_tree* tree, const void* element );

red_black_tree_iterator red_black_tree_upper_bound( red_black_tree* tree, const void* element );

void red_black_tree_erase( red_black_tree* tree, red_black_tree_iterator iter );

#define RED_BLACK_TREE_IS_EMPTY( tree ) \
    ((tree)->size == 0)

#define RED_BLACK_TREE_ITERATOR_START( iter, tree ) \
    iter = (tree)->root; \
    while ( (iter)->left ) \
    { \
        iter = (iter)->left; \
    }

#define RED_BLACK_TREE_ITERATOR_NEXT( iter ) \
    if ( (iter)->right ) \
    { \
        (iter) = (iter)->right; \
        while ( (iter)->left ) \
        { \
            iter = (iter)->left; \
        } \
    } \
    else \
    { \
        while ( (iter)->parent && (iter) == (iter)->parent->right ) \
        { \
            iter = (iter)->parent; \
        } \
        iter = (iter)->parent; \
    }

#define RED_BLACK_TREE_ITERATOR_IS_VALID( iter ) \
    ((iter) != NULL)

#define RED_BLACK_TREE_ITERATOR_GET( iter ) \
    ((iter) + 1)

typedef red_black_tree_node* red_black_tree_riterator;

#define RED_BLACK_TREE_RITERATOR_START( iter, tree ) \
    iter = (tree)->root; \
    while ( (iter)->right ) \
    { \
        iter = (iter)->right; \
    }

#define RED_BLACK_TREE_RITERATOR_NEXT( iter ) \
    if ( (iter)->left ) \
    { \
        (iter) = (iter)->left; \
        while ( (iter)->right ) \
        { \
            iter = (iter)->right; \
        } \
    } \
    else \
    { \
        while ( (iter)->parent && (iter) == (iter)->parent->left ) \
        { \
            iter = (iter)->parent; \
        } \
        iter = (iter)->parent; \
    }

#define RED_BLACK_TREE_RITERATOR_IS_VALID( iter ) \
    ((iter) != NULL)

#define RED_BLACK_TREE_RITERATOR_GET( iter ) \
    ((iter) + 1)

#endif /* defined( _RED_BLACK_TREE_H_ ) */
