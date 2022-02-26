#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l == NULL)
        return;
    struct list_head *node;
    struct list_head *safe;

    list_for_each_safe (node, safe, l) {
        element_t *p = list_entry(node, element_t, list);
        q_release_element(p);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }

    node->value = malloc((strlen(s) + 1) * sizeof(char));

    if (!node->value) {
        free(node);
        return false;
    }
    strncpy(node->value, s, strlen(s) + 1);
    list_add(&node->list, head);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }

    node->value = malloc((strlen(s) + 1) * sizeof(char));

    if (!node->value) {
        free(node);
        return false;
    }
    strncpy(node->value, s, strlen(s) + 1);
    list_add_tail(&node->list, head);

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head)) {
        return NULL;
    }

    element_t *removed_element = list_entry(head->next, element_t, list);
    list_del(head->next);

    if (sp && removed_element->value) {
        strncpy(sp, removed_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return removed_element;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head)) {
        return NULL;
    }

    element_t *removed_element = list_entry(head->prev, element_t, list);
    list_del(head->prev);

    if (sp && removed_element->value) {
        strncpy(sp, removed_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return removed_element;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (head == NULL || list_empty(head)) {
        return false;
    }

    struct list_head *fast = head->next, *slow = head->next;

    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_del(slow);
    element_t *p = list_entry(slow, element_t, list);
    q_release_element(p);

    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    if (!head) {
        return false;
    }

    element_t *cur;
    element_t *safe;
    bool duplicate = false;
    list_for_each_entry_safe (cur, safe, head, list) {
        if (cur->list.next != head &&
            !strcmp(cur->value,
                    list_entry(cur->list.next, element_t, list)->value)) {
            list_del(&cur->list);
            q_release_element(cur);
            duplicate = true;
        } else {
            if (duplicate) {
                list_del(&cur->list);
                q_release_element(cur);
                duplicate = false;
            }
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        if (node->next == head) {
            return;
        }
        list_del(node);
        list_add(node, safe);
        safe = node->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || !list_empty(head)) {
        return;
    }
    struct list_head *node, *safe;
    struct list_head *last = head->prev;

    list_for_each_safe (node, safe, head) {
        if (node == last) {
            return;
        }
        list_del(node);
        list_add(node, last);
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL;
    struct list_head **ptr = &head;

    for (; l1 && l2; ptr = &(*ptr)->next) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) < 0) {
            *ptr = l1;
            l1 = l1->next;
        } else {
            *ptr = l2;
            l2 = l2->next;
        }
    }
    *ptr = (struct list_head *) ((uintptr_t) l1 | (uintptr_t) l2);
    return head;
}
struct list_head *mergeSortList(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }

    struct list_head *fast = head->next, *slow = head;

    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    fast = slow->next;
    slow->next = NULL;
    slow = head;

    struct list_head *l1 = mergeSortList(slow);
    struct list_head *l2 = mergeSortList(fast);

    return merge(l1, l2);
}

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    head->prev->next = NULL;
    head->next = mergeSortList(head->next);

    struct list_head *cur = head->next;
    struct list_head *prev = head;

    while (cur != NULL) {
        cur->prev = prev;
        prev = cur;
        cur = cur->next;
    }
    prev->next = head;
    head->prev = prev;
}

void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    srand(time(NULL));
    struct list_head *old;
    struct list_head *new = head;

    for (int len = q_size(head); len > 0; len--) {
        int random = rand() % len;
        old = head->next;

        for (int i = 0; i < random; i++) {
            old = old->next;
        }
        new = new->prev;

        char *temp = list_entry(old, element_t, list)->value;
        list_entry(old, element_t, list)->value =
            list_entry(new, element_t, list)->value;
        list_entry(new, element_t, list)->value = temp;
    }
}