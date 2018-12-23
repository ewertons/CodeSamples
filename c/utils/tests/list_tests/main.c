#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "logging.h"

static int list_create_test()
{
    // Setup
    int result;

    // Run
    LIST_HANDLE list = list_create();

    // Verify
    result = (list != NULL ? 0 : 1);

    // Cleanup
    list_destroy(list);

    log_trace("Test list_create_test() [%s]", result == 0 ? "PASSED" : "FAILED");

    return result;
}

static int list_add_test()
{
    // Setup
    int result;
    LIST_HANDLE list = list_create();
    char* value0 = "abc";

    // Run
    LIST_NODE_HANDLE node = list_add(list, value0);

    // Verify
    result = (node != NULL ? 0 : 1);

    // Cleanup
    list_destroy(list);

    log_trace("Test list_add_test() [%s]", result == 0 ? "PASSED" : "FAILED");

    return result;
}

static int list_get_count_test()
{
    // Setup
    int result;
    LIST_HANDLE list;
    LIST_NODE_HANDLE node;
    char* value0 = "abc";
    size_t count;

    list = list_create();
    node = list_add(list, value0);

    // Run
    count = list_get_count(list);

    // Verify
    result = (count == 1 ? 0 : 1);

    // Cleanup
    list_destroy(list);

    log_trace("Test list_get_count_test() [%s]", result == 0 ? "PASSED" : "FAILED");

    return result;
}

static int list_get_head_node_test()
{
    // Setup
    int result;
    LIST_HANDLE list;
    LIST_NODE_HANDLE node;
    LIST_NODE_HANDLE head_node;
    char* value0 = "abc";
    size_t count;

    list = list_create();
    node = list_add(list, value0);

    // Run
    head_node = list_get_head_node(list);

    // Verify
    result = (head_node == node ? 0 : 1);

    // Cleanup
    list_destroy(list);

    log_trace("Test list_get_head_node_test() [%s]", result == 0 ? "PASSED" : "FAILED");

    return result;
}

static int list_get_node_value_test()
{
    // Setup
    int result;
    LIST_HANDLE list;
    LIST_NODE_HANDLE node;
    char* value0 = "abc";
    char* node_value;
    size_t count;

    list = list_create();
    node = list_add(list, value0);

    // Run
    node_value = (char*)list_get_node_value(node);

    // Verify
    result = (strcmp(node_value, value0) == 0 ? 0 : 1);

    // Cleanup
    list_destroy(list);

    log_trace("Test list_get_node_value_test() [%s]", result == 0 ? "PASSED" : "FAILED");

    return result;
}

static int list_get_node_value_test()
{
    // Setup
    int result;
    LIST_HANDLE list;
    LIST_NODE_HANDLE node;
    char* value0 = "abc";
    char* value1 = "def";
    char* value2 = "ghi";
    char* value3 = "jkl";
    char* values[4] = {value0, value1, value2, value3};
    size_t count = 0;

    list = list_create();
    (void)list_add(list, value0);
    (void)list_add(list, value1);
    (void)list_add(list, value2);
    (void)list_add(list, value3);

    // Run
    result = 0;
    node = list_get_head_node(list);

    while (node != NULL && result == 0)
    {
        char* node_value = (char*)list_get_node_value(node);

        // Verify
        result = (strcmp(node_value, values[count]) == 0 ? 0 : 1);

        node = list_get_next_node(node);
        count++;
    }

    if (result == 0)
    {
        
    }

    // Cleanup
    list_destroy(list);

    log_trace("Test list_get_node_value_test() [%s]", result == 0 ? "PASSED" : "FAILED");

    return result;
}

int main()
{
    log_trace("Running: list tests");

    int result = 0;;

    result += list_create_test();
    result += list_add_test();
    result += list_get_count_test();
    result += list_get_head_node_test();
    result += list_get_node_value_test();

    log_trace("Completed: list tests [%s]", result == 0 ? "PASSED" : "FAILED");

    return result;
}