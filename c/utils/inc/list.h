
#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

typedef void* LIST_HANDLE;
typedef void* LIST_NODE_HANDLE;

typedef bool (*LIST_FIND_MATCH_FUNCTION)(LIST_NODE_HANDLE list_node_handle, const void* context);
typedef bool (*LIST_REMOVE_MATCH_FUNCTION)(LIST_NODE_HANDLE list_node_handle, bool* continue_processing, const void* context);
typedef bool (*LIST_FOREACH_FUNCTION)(void* value, const void* context);

extern LIST_HANDLE list_create();
extern LIST_NODE_HANDLE list_add(LIST_HANDLE list_handle, void* value);
extern int list_remove(LIST_HANDLE list_handle, LIST_NODE_HANDLE list_node_handle);
extern size_t list_get_count(LIST_HANDLE list_handle);
extern LIST_NODE_HANDLE list_get_head_node(LIST_HANDLE list_handle);
extern LIST_NODE_HANDLE list_get_next_node(LIST_NODE_HANDLE list_node_handle);
extern void* list_get_node_value(LIST_NODE_HANDLE list_node_handle);
extern LIST_NODE_HANDLE list_find(LIST_HANDLE list_handle, LIST_FIND_MATCH_FUNCTION match_function, const void* context);
extern int list_remove_if(LIST_HANDLE list_handle, LIST_REMOVE_MATCH_FUNCTION match_function, const void* context);
extern int list_foreach(LIST_HANDLE list_handle, LIST_FOREACH_FUNCTION action_function, const void* context);
extern void list_destroy(LIST_HANDLE list_handle);

#endif // LIST_H
