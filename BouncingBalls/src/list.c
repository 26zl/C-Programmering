/*
 * Singly linked list implementation with a simple iterator interface.
 */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/* Internal list node and list definitions. */

typedef struct listnode listnode_t;

struct listnode {
    listnode_t  *next;
    void        *item;
};

struct list {
    listnode_t *head;
    int numitems;
};

/* Return a newly created, empty list. */
list_t *list_create(void)
{
    list_t *list = malloc(sizeof(*list));

    if (!list) {
        return NULL;
    }

    list->head = NULL;
    list->numitems = 0;

    return list;
}

/* Free the list and its nodes, but not the stored items. */
void list_destroy(list_t *list)
{
    listnode_t *node;

    if (!list) {
        return;
    }

    node = list->head;
    while (node) {
        listnode_t *next = node->next;
        free(node);
        node = next;
    }

    free(list);
}

/* Add an item to the front of the list. */
void list_addfirst(list_t *list, void *item)
{
    listnode_t *node;

    if (!list) {
        return;
    }

    node = malloc(sizeof(*node));
    if (!node) {
        return;
    }

    node->item = item;
    node->next = list->head;
    list->head = node;
    list->numitems++;
}

/* Add an item to the end of the list. */
void list_addlast(list_t *list, void *item)
{
    listnode_t *node;
    listnode_t *tail;

    if (!list) {
        return;
    }

    if (!list->head) {
        /* Empty list: reuse addfirst so bookkeeping stays consistent. */
        list_addfirst(list, item);
        return;
    }

    tail = list->head;
    while (tail->next) { 
        /* Walk to the current end before appending. */
        tail = tail->next;
    }

    node = malloc(sizeof(*node));
    if (!node) {
        return;
    }

    node->item = item;
    node->next = NULL;
    tail->next = node;
    list->numitems++;
}

/* Remove the first occurrence of the item from the list; does not free the item itself. */
void list_remove(list_t *list, void *item)
{
    listnode_t *node;
    listnode_t *prev;

    if (!list) {
        return;
    }

    node = list->head;
    prev = NULL;

    while (node) {
        if (node->item == item) {
            if (prev) {
                prev->next = node->next;
            } else {
                list->head = node->next;
            }

            /* Unlink the node; stored item lifetime is managed by caller. */
            free(node);
            list->numitems--;
            return;
        }

        prev = node;
        node = node->next;
    }
}

/* Return the number of items currently stored in the list. */
int list_size(list_t *list)
{
    return list ? list->numitems : 0;
}



/* Iterator implementation */
 struct list_iterator {
    listnode_t *next;
    list_t *list;
};


/* Return a newly created list iterator for the given list. */
list_iterator_t *list_createiterator(list_t *list)
{
    list_iterator_t *iter = malloc(sizeof(*iter));

    if (!iter) {
        return NULL;
    }

    iter->list = list;
    iter->next = list ? list->head : NULL;

    return iter;
}


/* Free the memory for the given list iterator. */
void list_destroyiterator(list_iterator_t *iter)
{
    free(iter);
}


/* Move iterator to next item in list and return the current item. */
void *list_next(list_iterator_t *iter)
{
    listnode_t *current;

    if (!iter || !iter->next) {
        return NULL;
    }

    current = iter->next;
    iter->next = current->next; /* Advance before returning to allow safe removal. */

    return current->item;
}


/* Reset iterator so it again points to the first item in the list. */
void list_resetiterator(list_iterator_t *iter)
{
    if (!iter) {
        return;
    }

    iter->next = iter->list ? iter->list->head : NULL;
}
