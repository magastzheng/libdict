/*
 * libdict - common definitions for binary search trees.
 *
 * Copyright (c) 2001-2014, Farooq Mela
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "tree_common.h"

#include <string.h>
#include "dict_private.h"

typedef struct tree_node {
    TREE_NODE_FIELDS(struct tree_node);
} tree_node;

typedef struct {
    TREE_FIELDS(tree_node);
} tree;

typedef struct {
    TREE_ITERATOR_FIELDS(tree, tree_node);
} tree_iterator;

void
tree_node_rot_left(void* Tree, void* Node)
{
    tree_node* const n = Node;
    tree_node* const nr = n->rlink;
    ASSERT(nr != NULL);
    if ((n->rlink = nr->llink) != NULL)
	n->rlink->parent = n;
    nr->llink = n;

    tree_node* p = n->parent;
    n->parent = nr;
    if ((nr->parent = p) != NULL) {
	if (p->llink == n)
	    p->llink = nr;
	else
	    p->rlink = nr;
    } else {
	((tree*)Tree)->root = nr;
    }
}

void
tree_node_rot_right(void* Tree, void* Node)
{
    tree_node* const n = Node;
    tree_node* const nl = n->llink;
    ASSERT(nl != NULL);
    if ((n->llink = nl->rlink) != NULL)
	n->llink->parent = n;
    nl->rlink = n;

    tree_node* const p = n->parent;
    n->parent = nl;
    if ((nl->parent = p) != NULL) {
	if (p->llink == n)
	    p->llink = nl;
	else
	    p->rlink = nl;
    } else {
	((tree*)Tree)->root = nl;
    }
}

void*
tree_node_prev(void* Node)
{
    tree_node* node = Node;
    if (node->llink)
	return tree_node_max(node->llink);
    tree_node* parent = node->parent;
    while (parent && parent->llink == node) {
	node = parent;
	parent = parent->parent;
    }
    return parent;
}

void*
tree_node_next(void* Node)
{
    tree_node* node = Node;
    if (node->rlink)
	return tree_node_min(node->rlink);
    tree_node* parent = node->parent;
    while (parent && parent->rlink == node) {
	node = parent;
	parent = parent->parent;
    }
    return parent;
}

void*
tree_node_min(void* Node)
{
    tree_node* node = Node;
    if (!node)
	return NULL;
    while (node->llink)
	node = node->llink;
    return node;
}

void*
tree_node_max(void* Node)
{
    tree_node* node = Node;
    if (!node)
	return NULL;
    while (node->rlink)
	node = node->rlink;
    return node;
}

void*
tree_search_node(void* Tree, const void* key)
{
    tree* t = Tree;
    for (tree_node* node = t->root; node;) {
	int cmp = t->cmp_func(key, node->key);
	if (cmp < 0)
	    node = node->llink;
	else if (cmp)
	    node = node->rlink;
	else
	    return node;
    }
    return NULL;
}

void**
tree_search(void* Tree, const void* key)
{
    tree_node* node = tree_search_node(Tree, key);
    return node ? &node->datum : NULL;
}

void*
tree_search_le_node(void* Tree, const void* key)
{
    tree* t = Tree;
    tree_node* node = t->root, *ret = NULL;
    while (node) {
	int cmp = t->cmp_func(key, node->key);
	if (cmp == 0)
	    return node;
	if (cmp < 0) {
	    node = node->llink;
	} else {
	    ret = node;
	    node = node->rlink;
	}
    }
    return ret;
}

void**
tree_search_le(void* Tree, const void* key)
{
    tree_node* node = tree_search_le_node(Tree, key);
    return node ? &node->datum : NULL;
}

void*
tree_search_lt_node(void* Tree, const void* key)
{
    tree* t = Tree;
    tree_node* node = t->root, *ret = NULL;
    while (node) {
	int cmp = t->cmp_func(key, node->key);
	if (cmp <= 0) {
	    node = node->llink;
	} else {
	    ret = node;
	    node = node->rlink;
	}
    }
    return ret;
}

void**
tree_search_lt(void* Tree, const void* key)
{
    tree_node* node = tree_search_lt_node(Tree, key);
    return node ? &node->datum : NULL;
}

void*
tree_search_ge_node(void* Tree, const void* key)
{
    tree* t = Tree;
    tree_node* node = t->root, *ret = NULL;
    while (node) {
	int cmp = t->cmp_func(key, node->key);
	if (cmp == 0) {
	    return node;
	}
	if (cmp < 0) {
	    ret = node;
	    node = node->llink;
	} else {
	    node = node->rlink;
	}
    }
    return ret;
}

void**
tree_search_ge(void* Tree, const void* key)
{
    tree_node* node = tree_search_ge_node(Tree, key);
    return node ? &node->datum : NULL;
}

void*
tree_search_gt_node(void* Tree, const void* key)
{
    tree* t = Tree;
    tree_node* node = t->root, *ret = NULL;
    while (node) {
	int cmp = t->cmp_func(key, node->key);
	if (cmp < 0) {
	    ret = node;
	    node = node->llink;
	} else {
	    node = node->rlink;
	}
    }
    return ret;
}

void**
tree_search_gt(void* Tree, const void* key)
{
    tree_node* node = tree_search_gt_node(Tree, key);
    return node ? &node->datum : NULL;
}

size_t
tree_traverse(void* Tree, dict_visit_func visit)
{
    ASSERT(visit != NULL);

    tree* t = Tree;
    size_t count = 0;
    if (t->root) {
	tree_node* node = tree_node_min(t->root);
	do {
	    ++count;
	    if (!visit(node->key, node->datum))
		break;
	    node = tree_node_next(node);
	} while (node);
    }
    return count;
}

bool
tree_select(void *Tree, size_t n, const void **key, void **datum)
{
    tree* t = Tree;
    if (n >= t->count) {
	*key = NULL;
	*datum = NULL;
	return false;
    }
    tree_node* node;
    if (n >= t->count / 2) {
	node = tree_node_max(t->root);
	n = t->count - 1 - n;
	while (n--)
	    node = tree_node_prev(node);
    } else {
	node = tree_node_min(t->root);
	while (n--)
	    node = tree_node_next(node);
    }
    *key = node->key;
    *datum = node->datum;
    return true;
}

static void
tree_node_free(tree_node* node, dict_delete_func delete_func)
{
    tree_node* const llink = node->llink;
    tree_node* const rlink = node->rlink;
    if (delete_func)
	delete_func(node->key, node->datum);
    FREE(node);
    if (llink)
	tree_node_free(llink, delete_func);
    if (rlink)
	tree_node_free(rlink, delete_func);
}

size_t
tree_count(const void* Tree)
{
    return ((const tree*)Tree)->count;
}

size_t
tree_clear(void* Tree, dict_delete_func delete_func)
{
    tree* t = Tree;
    const size_t count = t->count;
    if (t->root) {
	tree_node_free(t->root, delete_func);
	t->root = NULL;
	t->count = 0;
    }
    return count;
}

size_t
tree_free(void* Tree, dict_delete_func delete_func)
{
    const size_t count = tree_clear(Tree, delete_func);
    FREE(Tree);
    return count;
}

static size_t
node_min_path_length(const tree_node* node)
{
    size_t l = node->llink ? node_min_path_length(node->llink) : 0;
    size_t r = node->rlink ? node_min_path_length(node->rlink) : 0;
    return 1 + MIN(l, r);
}

size_t
tree_min_path_length(const void* Tree)
{
    const tree* t = Tree;
    return t->root ? node_min_path_length(t->root) : 0;
}

static size_t
node_max_path_length(const tree_node* node)
{
    size_t l = node->llink ? node_min_path_length(node->llink) + 1 : 0;
    size_t r = node->rlink ? node_min_path_length(node->rlink) + 1 : 0;
    return 1 + MAX(l, r);
}

size_t
tree_max_path_length(const void* Tree)
{
    const tree* t = Tree;
    return t->root ? node_max_path_length(t->root) : 0;
}

static size_t
node_path_length(const tree_node* node, size_t level)
{
    return level
	+ (node->llink ? node_path_length(node->llink, level + 1) : 0)
	+ (node->rlink ? node_path_length(node->rlink, level + 1) : 0);
}

size_t
tree_total_path_length(const void* Tree)
{
    const tree* t = Tree;
    return t->root ? node_path_length(t->root, 1) : 0;
}

bool
tree_iterator_valid(const void* Iterator)
{
    return ((const tree_iterator*)Iterator)->node != NULL;
}

void
tree_iterator_invalidate(void* Iterator)
{
    ((tree_iterator*)Iterator)->node = NULL;
}

void
tree_iterator_free(void* Iterator)
{
    tree_iterator* iterator = Iterator;
    iterator->node = NULL;
    FREE(iterator);
}

bool
tree_iterator_next(void* Iterator)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node != NULL) && (iterator->node = tree_node_next(iterator->node)) != NULL;
}

bool
tree_iterator_prev(void* Iterator)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node != NULL) && (iterator->node = tree_node_prev(iterator->node)) != NULL;
}

bool
tree_iterator_next_n(void* Iterator, size_t count)
{
    tree_iterator* iterator = Iterator;
    while (iterator->node && count--)
	iterator->node = tree_node_next(iterator->node);
    return iterator->node != NULL;
}

bool
tree_iterator_prev_n(void* Iterator, size_t count)
{
    tree_iterator* iterator = Iterator;
    while (iterator->node && count--)
	iterator->node = tree_node_prev(iterator->node);
    return iterator->node != NULL;
}

bool
tree_iterator_first(void* Iterator)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node = tree_node_min(iterator->tree->root)) != NULL;
}

bool
tree_iterator_last(void* Iterator)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node = tree_node_max(iterator->tree->root)) != NULL;
}

bool
tree_iterator_search(void* Iterator, const void* key)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node = tree_search_node(iterator->tree, key)) != NULL;
}

bool
tree_iterator_search_le(void* Iterator, const void* key)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node = tree_search_le_node(iterator->tree, key)) != NULL;
}

bool
tree_iterator_search_lt(void* Iterator, const void* key)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node = tree_search_lt_node(iterator->tree, key)) != NULL;
}

bool
tree_iterator_search_ge(void* Iterator, const void* key)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node = tree_search_ge_node(iterator->tree, key)) != NULL;
}

bool
tree_iterator_search_gt(void* Iterator, const void* key)
{
    tree_iterator* iterator = Iterator;
    return (iterator->node = tree_search_gt_node(iterator->tree, key)) != NULL;
}

const void*
tree_iterator_key(const void* Iterator)
{
    const tree_iterator* iterator = Iterator;
    return iterator->node ? iterator->node->key : NULL;
}

void**
tree_iterator_datum(void* Iterator)
{
    tree_iterator* iterator = Iterator;
    return iterator->node ? &iterator->node->datum : NULL;
}
