#include "list.h"
#include "logging.h"
#include "result.h"

typedef struct LIST_NODE_STRUCT
{
    void* value;
    struct LIST_NODE_STRUCT* next;
} LIST_NODE;

typedef struct LIST_STRUCT
{
    size_t count;
    LIST_NODE* head;
    LIST_NODE* tail;
} LIST;


LIST_HANDLE list_create()
{
    LIST* result;

    if ((result = calloc(1, sizeof(LIST))) == NULL)
    {
        log_error("Failed allocating LIST structure");
    }

    return result;
}

LIST_NODE_HANDLE list_add(LIST_HANDLE list_handle, void* value)
{
    LIST_NODE* result;

    if (list_handle == NULL)
    {
        log_error("Invalid argument (list_handle is NULL)");
        result = NULL;
    }
    else
    {
        if ((result = malloc(sizeof(LIST_NODE))) == NULL)
        {
            log_error("Failed allocating LIST_NODE structure");
        }
        else
        {
            LIST* list = (LIST*)list_handle;

            result->value = value;
            result->next = NULL;

            if (list->tail != NULL)
            {
                list->tail->next = result;
            }
            else
            {
                list->head = result;
            }

            list->tail = result;
            list->count++;
        }
    }

    return result;
}

int list_remove(LIST_HANDLE list_handle, LIST_NODE_HANDLE list_node_handle)
{
    int result;

    if (list_handle == NULL || list_node_handle == NULL)
    {
        log_error("Invalid argument (list_handle=%p, list_node_handle=%p)", list_handle, list_node_handle);
        result = __FAILURE__;
    }
    else
    {
        LIST* list = (LIST*)list_handle;
        LIST_NODE* previous = NULL;
        LIST_NODE* current = list->head;

        result = __FAILURE__;

        while (current != NULL)
        {
            if (current == list_node_handle)
            {
                if (previous == NULL)
                {
                    // The first node of the list is being removed.
                    list->head = current->next;

                    if (list->tail == current)
                    {
                        // There is only one node in this list.
                        list->tail = NULL;
                    }
                }
                else
                {
                    previous->next = current->next;
                }

                free(current);
                list->count--;
                result = 0;
                break;
            }
        }

        if (result != 0)
        {
            log_error("List node is not part of this list.");
        }
    }

    return result;
}

size_t list_get_count(LIST_HANDLE list_handle)
{
    size_t result;

    if (list_handle == NULL)
    {
        log_error("Invalid argument (list_handle is NULL)");
        result = 0;
    }
    else
    {
        LIST* list = (LIST*)list_handle;
        result = list->count;
    }

    return result;
}

LIST_NODE_HANDLE list_get_head_node(LIST_HANDLE list_handle)
{
    LIST_NODE* result;

    if (list_handle == NULL)
    {
        log_error("Invalid argument (list_handle is NULL)");
        result = NULL;
    }
    else
    {
        LIST* list = (LIST*)list_handle;
        result = list->head;
    }

    return result;
}

LIST_NODE_HANDLE list_get_next_node(LIST_NODE_HANDLE list_node_handle)
{
    LIST_NODE* result;

    if (list_node_handle == NULL)
    {
        log_error("Invalid argument (list_node_handle is NULL)");
        result = NULL;
    }
    else
    {
        LIST_NODE* list_node = (LIST_NODE*)list_node_handle;
        result = list_node->next;
    }

    return result;
}

void* list_get_node_value(LIST_NODE_HANDLE list_node_handle)
{
    void* result;

    if (list_node_handle == NULL)
    {
        log_error("Invalid argument (list_node_handle is NULL)");
        result = NULL;
    }
    else
    {
        LIST_NODE* list_node = (LIST_NODE*)list_node_handle;
        result = list_node->value;
    }

    return result;
}

LIST_NODE_HANDLE list_find(LIST_HANDLE list_handle, LIST_FIND_MATCH_FUNCTION match_function, const void* context)
{
    LIST_NODE* result;

    if (list_handle == NULL || match_function == NULL)
    {
        log_error("Invalid argument (list_handle=%p, match_function=%p)", list_handle, match_function);
        result = NULL;
    }
    else
    {
        LIST* list = (LIST*)list_handle;
        result = list->head;

        while (result != NULL)
        {
            if (match_function(result, context))
            {
                break;
            }
        }
    }

    return result;
}

int list_remove_if(LIST_HANDLE list_handle, LIST_REMOVE_MATCH_FUNCTION match_function, const void* context)
{
    int result;

    if (list_handle == NULL || match_function == NULL)
    {
        log_error("Invalid argument (list_handle=%p, match_function=%p)", list_handle, match_function);
        result = __FAILURE__;
    }
    else
    {
        LIST* list = (LIST*)list_handle;
        LIST_NODE* previous = NULL;
        LIST_NODE* current = list->head;
        bool continue_processing = true;

        while (current != NULL && continue_processing)
        {
            LIST_NODE* next = current->next;

            if (match_function(current, &continue_processing, context))
            {
                if (previous == NULL)
                {
                    list->head = list->head->next;
                }
                else
                {
                    previous->next = current->next;
                }

                free(current);
                list->count--;
            }
            else
            {
                previous = current;
            }
            
            current = next;
        }

        result = 0;
    }

    return result;
}

int list_foreach(LIST_HANDLE list_handle, LIST_FOREACH_FUNCTION action_function, const void* context)
{
    int result;

    if (list_handle == NULL || action_function == NULL)
    {
        log_error("Invalid argument (list_handle=%p, action_function=%p)", list_handle, action_function);
        result = __FAILURE__;
    }
    else
    {
        LIST* list = (LIST*)list_handle;
        LIST_NODE* current = list->head;

        while (current != NULL)
        {
            if (!action_function(current->value, context))
            {
                break;
            }
        }

        result = 0;
    }

    return result;
}

void list_destroy(LIST_HANDLE list_handle)
{
    if (list_handle == NULL)
    {
        log_error("Invalid argument (list_handle is NULL)");
    }
    else
    {
        LIST* list = (LIST*)list_handle;
        LIST_NODE* current = list->head;

        while (current != NULL)
        {
            LIST_NODE* temp = current;
            current = current->next;
            free(temp);
        }

        free(list);
    }
}
