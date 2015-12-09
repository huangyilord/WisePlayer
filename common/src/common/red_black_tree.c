//
//  red_black_tree.c
//  common
//
//  Created by Yi Huang on 28/4/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "red_black_tree.h"

#include "common_debug.h"

/*-----------------------------------------------------------
 |   node           right
 |   / \    ==>     / \
 |   a  right     node  y
 |       / \       / \
 |       b  y     a   b
 -----------------------------------------------------------*/
static red_black_tree_node* rotate_left( red_black_tree_node* node, red_black_tree_node* root )
{
    red_black_tree_node* right = node->right;    //right<--node->right
    if ((node->right = right->left))
    {
        right->left->parent = node;
    }
    right->left = node;
    if ((right->parent = node->parent))
    {
        if (node == node->parent->right)
        {
            node->parent->right = right;
        }
        else
        {
            node->parent->left = right;
        }
    }
    else
    {
        root = right;
    }
    node->parent = right;
    return root;
}

/*-----------------------------------------------------------
 |       node            left
 |       / \             / \
 |    left  y   ==>    a    node
 |   / \                    / \
 |  a   b                  b   y
 -----------------------------------------------------------*/
static red_black_tree_node* rotate_right( red_black_tree_node* node, red_black_tree_node* root )
{
    red_black_tree_node* left = node->left;
    if ((node->left = left->right))
    {
        left->right->parent = node;
    }
    left->right = node;
    if ((left->parent = node->parent))
    {
        if (node == node->parent->right)
        {
            node->parent->right = left;
        }
        else
        {
            node->parent->left = left;
        }
    }
    else
    {
        root = left;
    }
    node->parent = left;
    return root;
}

/*static red_black_tree_node* search( red_black_tree* tree, const void* element, red_black_tree_node* root, red_black_tree_node** pparent )
{
    red_black_tree_node *node = root, *parent = NULL;
    int ret;
    while (node)
    {
        parent = node;
        ret = tree->compare_fun( tree, node + 1, element );
        if ( 0 < ret )
        {
            node = node->left;
        }
        else if ( 0 > ret )
        {
            node = node->right;
        }
        else
        {
            return node;
        }
    }
    if (pparent)
    {
        *pparent = parent;
    }
    return NULL;
}*/

static red_black_tree_node* search_lower_bound( red_black_tree* tree, const void* element, red_black_tree_node* root )
{
    red_black_tree_node* node = root;
    red_black_tree_node* bound = node;
    int32 compare_result = 0;
    while ( node )
    {
        compare_result = tree->compare_fun( tree, node + 1, element );
        if ( 0 > compare_result )
        {
            node = node->right;
        }
        else
        {
            bound = node;
            node = node->left;
        }
    }
    return bound;
}

static red_black_tree_node* search_upper_bound( red_black_tree* tree, const void* element, red_black_tree_node* root )
{
    red_black_tree_node* node = root;
    red_black_tree_node* bound = NULL;
    int32 compare_result = 0;
    while ( node )
    {
        compare_result = tree->compare_fun( tree, node + 1, element );
        if ( 0 < compare_result )
        {
            bound = node;
            node = node->left;
        }
        else
        {
            node = node->right;
        }
    }
    return bound;
}

static red_black_tree_node* insert_rebalance( red_black_tree_node *node, red_black_tree_node *root )
{
    red_black_tree_node *parent, *gparent, *uncle, *tmp;
    while ((parent = node->parent) && parent->type == red_black_tree_node_type_red)
    {
        gparent = parent->parent;
        if ( parent == gparent->left )
        {
            uncle = gparent->right;
            if (uncle && uncle->type == red_black_tree_node_type_red)
            {
                uncle->type = red_black_tree_node_type_black;
                parent->type = red_black_tree_node_type_black;
                gparent->type = red_black_tree_node_type_red;
                node = gparent;
            }
            else
            {
                if (parent->right == node)
                {
                    root = rotate_left(parent, root);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->type = red_black_tree_node_type_black;
                gparent->type = red_black_tree_node_type_red;
                root = rotate_right(gparent, root);
            }
        }
        else
        {
            uncle = gparent->left;
            if (uncle && uncle->type == red_black_tree_node_type_red)
            {
                uncle->type = red_black_tree_node_type_black;
                parent->type = red_black_tree_node_type_black;
                gparent->type = red_black_tree_node_type_red;
                node = gparent;
            }
            else
            {
                if (parent->left == node)
                {
                    root = rotate_right(parent, root);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->type = red_black_tree_node_type_black;
                gparent->type = red_black_tree_node_type_red;
                root = rotate_left(gparent, root);
            }
        }
    }
    root->type = red_black_tree_node_type_black;
    return root;
}

static red_black_tree_node* erase_rebalance( red_black_tree_node *node, red_black_tree_node *parent, red_black_tree_node *root )
{
    red_black_tree_node *other, *o_left, *o_right;
    while ((!node || node->type == red_black_tree_node_type_black) && node != root)
    {
        if ( parent->left == node )
        {
            other = parent->right;
            if ( other->type == red_black_tree_node_type_red )
            {
                other->type = red_black_tree_node_type_black;
                parent->type = red_black_tree_node_type_red;
                root = rotate_left( parent, root );
                other = parent->right;
            }
            if ((!other->left || other->left->type == red_black_tree_node_type_black) &&
                (!other->right || other->right->type == red_black_tree_node_type_black))
            {
                other->type = red_black_tree_node_type_red;
                node = parent;
                parent = node->parent;
            }
            else
            {
                if (!other->right || other->right->type == red_black_tree_node_type_black)
                {
                    if ((o_left = other->left))
                    {
                        o_left->type = red_black_tree_node_type_black;
                    }
                    other->type = red_black_tree_node_type_red;
                    root = rotate_right( other, root );
                    other = parent->right;
                }
                other->type = parent->type;
                parent->type = red_black_tree_node_type_black;
                if (other->right)
                {
                    other->right->type = red_black_tree_node_type_black;
                }
                root = rotate_left( parent, root );
                node = root;
                break;
            }
        }
        else
        {
            other = parent->left;
            if ( other->type == red_black_tree_node_type_red )
            {
                other->type = red_black_tree_node_type_black;
                parent->type = red_black_tree_node_type_red;
                root = rotate_right( parent, root );
                other = parent->left;
            }
            if ( (!other->left || other->left->type == red_black_tree_node_type_black) &&
                (!other->right || other->right->type == red_black_tree_node_type_black) )
            {
                other->type = red_black_tree_node_type_red;
                node = parent;
                parent = node->parent;
            }
            else
            {
                if ( !other->left || other->left->type == red_black_tree_node_type_black )
                {
                    if ((o_right = other->right))
                    {
                        o_right->type = red_black_tree_node_type_black;
                    }
                    other->type = red_black_tree_node_type_red;
                    root = rotate_left( other, root );
                    other = parent->left;
                }
                other->type = parent->type;
                parent->type = red_black_tree_node_type_black;
                if (other->left)
                {
                    other->left->type = red_black_tree_node_type_black;
                }
                root = rotate_right( parent, root );
                node = root;
                break;
            }
        }
    }
    if ( node )
    {
        node->type = red_black_tree_node_type_black;
    }
    return root;
}

static int32 default_compare_fun( red_black_tree* tree, const void* element1, const void* element2 )
{
    int32 *e1, *e2;
    uint32 i = 0;
    for ( i = 0; i < tree->element_size; i += sizeof(int32) )
    {
        e1 = (int32*)((byte*)element1 + i);
        e2 = (int32*)((byte*)element2 + i);
        if ( *e1 != *e2 )
        {
            return *e1 - *e2;
        }
    }
    return 0;
}

boolean red_black_tree_initialize( red_black_tree* tree, size_t element_size, memory_pool* pool )
{
    if ( !object_pool_initialize( &tree->node_pool, pool, sizeof(red_black_tree_node) + element_size ) )
    {
        return FALSE;
    }
    tree->root = NULL;
    tree->element_size = element_size;
    tree->size = 0;
    tree->compare_fun = default_compare_fun;
    return TRUE;
}

void red_black_tree_destroy( red_black_tree* tree )
{
    red_black_tree_clear( tree );
    object_pool_destroy( &tree->node_pool );
}

static void release_node( red_black_tree* tree, red_black_tree_node* node )
{
    if ( node->left )
    {
        release_node( tree, node->left );
    }
    if ( node->right )
    {
        release_node( tree, node->right );
    }
    object_pool_free( &tree->node_pool, node );
}

void red_black_tree_clear( red_black_tree* tree )
{
    if ( tree->root )
    {
        ASSERT( tree->size > 0 );
        release_node( tree, tree->root );
        tree->root = NULL;
        tree->size = 0;
    }
}

red_black_tree_iterator red_black_tree_find( red_black_tree* tree, const void* element )
{
    red_black_tree_node* node = search_lower_bound( tree, element, tree->root );
    if ( NULL == node || tree->compare_fun( tree, node + 1, element ) != 0 )
    {
        return NULL;
    }
    return node;
}

red_black_tree_iterator red_black_tree_lower_bound( red_black_tree* tree, const void* element )
{
    return search_lower_bound( tree, element, tree->root );
}

red_black_tree_iterator red_black_tree_upper_bound( red_black_tree* tree, const void* element )
{
    return search_upper_bound( tree, element, tree->root );
}

red_black_tree_iterator red_black_tree_insert( red_black_tree* tree, const void* element )
{
    red_black_tree_node* node = NULL;
    red_black_tree_node* parent = NULL;
    node = object_pool_alloc( &tree->node_pool );
    if ( NULL == node )
    {
        return FALSE;
    }
    memcpy( node + 1, element, tree->element_size );
    node->parent = NULL;
    node->left = node->right = NULL;
    node->type = red_black_tree_node_type_red;
    if ( NULL == tree->root )
    {
        tree->root = node;
        tree->root = insert_rebalance( node, tree->root );
        ++ tree->size;
        return node;
    }
    parent = search_upper_bound( tree, element, tree->root );
    if ( NULL != parent )
    {
        if ( NULL == parent->left )
        {
            node->parent = parent;
            parent->left = node;
            tree->root = insert_rebalance( node, tree->root );
            ++tree->size;
            return node;
        }
        else
        {
            parent = parent->left;
        }
    }
    else
    {
        parent = tree->root;
    }
    while ( parent->right )
    {
        parent = parent->right;
    }
    node->parent = parent;
    parent->right = node;
    tree->root = insert_rebalance( node, tree->root );
    ++tree->size;
    return node;
}

void red_black_tree_remove( red_black_tree* tree, const void* element )
{
    red_black_tree_node* node = red_black_tree_find( tree, element );
    if ( NULL == node )
    {
        // not exist
        return;
    }
    red_black_tree_erase( tree, node );
}

void red_black_tree_erase( red_black_tree* tree, red_black_tree_iterator iter )
{
    red_black_tree_node *child, *parent, *old, *left;
    red_black_tree_node_type type;
    old = iter;
    if (iter->left && iter->right)
    {
        iter = iter->right;
        while ((left = iter->left) != NULL)
        {
            iter = left;
        }
        child = iter->right;
        parent = iter->parent;
        type = iter->type;
        if (child)
        {
            child->parent = parent;
        }
        if (parent)
        {
            if (parent->left == iter)
            {
                parent->left = child;
            }
            else
            {
                parent->right = child;
            }
        }
        else
        {
            tree->root = child;
        }
        
        if (iter->parent == old)
        {
            parent = iter;
        }
        
        iter->parent = old->parent;
        iter->type = old->type;
        iter->right = old->right;
        iter->left = old->left;
        
        if (old->parent)
        {
            if (old->parent->left == old)
            {
                old->parent->left = iter;
            }
            else
            {
                old->parent->right = iter;
            }
        }
        else
        {
            tree->root = iter;
        }
        
        old->left->parent = iter;
        if (old->right)
        {
            old->right->parent = iter;
        }
    }
    else
    {
        if (!iter->left)
        {
            child = iter->right;
        }
        else if (!iter->right)
        {
            child = iter->left;
        }
        parent = iter->parent;
        type = iter->type;
        if (child)
        {
            child->parent = parent;
        }
        if (parent)
        {
            if (parent->left == iter)
            {
                parent->left = child;
            }
            else
            {
                parent->right = child;
            }
        }
        else
        {
            tree->root = child;
        }
    }
    object_pool_free( &tree->node_pool, old );
    if (type == red_black_tree_node_type_black)
    {
        tree->root = erase_rebalance( child, parent, tree->root );
    }
    -- tree->size;
}

