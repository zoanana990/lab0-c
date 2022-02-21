#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 * cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    // make an element
    struct list_head *l = malloc(sizeof(struct list_head));

    // if not malloc memory
    if (!l)
        return NULL;

    // Initial the list node
    INIT_LIST_HEAD(l);
    return l;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *curr = l->next;
    while (curr != l) {
        struct list_head *prev = curr;
        curr = curr->next;
        list_del(prev);
        q_release_element(list_entry(prev, element_t, list));
    }
    free(l);
}

/*
 * Due to the repeated code in q_insert_head and q_insert_tail
 * write a functionto replace them
 */

element_t *q_insert(char *s)
{
    element_t *q = malloc(sizeof(element_t));
    if (q == NULL) {
        return NULL;
    }

    int len = strlen(s) + 1;
    q->value = malloc(sizeof(char) * len);
    if (!q->value) {
        free(q);
        return NULL;
    }
    strncpy(q->value, s, len - 1);
    q->value[len - 1] = '\0';
    return q;
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
    // Boundary Condition
    if (!head)
        return false;

    // make a new element
    element_t *q = q_insert(s);

    if (!q)
        return false;

    list_add(&q->list, head);

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
    // Boundary Condition
    if (!head)
        return false;

    // make a new element
    element_t *q = q_insert(s);

    if (!q)
        return false;

    list_add_tail(&q->list, head);

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
    if (!head || list_empty(head))
        return NULL;

    element_t *q_head = list_first_entry(head, element_t, list);
    list_del_init(&q_head->list);

    if (sp) {
        strncpy(sp, q_head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return q_head;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove_head(head->prev->prev, sp, bufsize);
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
    if (!head || list_empty(head))
        return false;

    struct list_head *fast = head->next, *slow = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;

    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));

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
    // // Boundary Condition
    // if (!head || list_empty(head))
    //     return false;

    // // indirect pointer and get the queue of this node
    // struct list_head *curr = head->next, *prev = head;

    // while (curr != head && curr->next != head) {

    //     element_t *q_curr = list_entry(curr, element_t, list);
    //     element_t *q_next = list_entry(curr->next, element_t, list);

    //     // if it is duplicate node
    //     if (curr->next && strcmp(q_next->value, q_curr->value)) {

    //         struct list_head *temp = curr;
    //         element_t *q_temp = list_entry(temp, element_t, list);

    //         while (temp && strcmp(q_temp->value, q_curr->value)) {
    //             struct list_head *prev = temp;
    //             temp = temp->next;

    //             // take out the node and delete queue
    //             list_del(prev);
    //             q_release_element(list_entry(prev, element_t, list));

    //             // update the list entry
    //             q_temp = list_entry(temp, element_t, list);
    //         }

    //         curr = temp;
    //         curr->prev = prev;
    //         prev->next = curr;
    //     }

    //     // normal traversal
    //     else{
    //         curr = curr->next;
    //     }

    // }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    struct list_head **indirect = &head->next, *future = NULL, *current = NULL;
    while (*indirect != head && (*indirect)->next != head) {
        current = *indirect;
        future = (*indirect)->next;

        // swap two node
        current->next = future->next;
        future->prev = current->prev;
        future->next = current;
        current->prev = future;

        // link the prev->next node and next->prev node
        future->prev->next = future;
        current->next->prev = current;

        // Substitute the position
        *indirect = future;

        // go next two node
        indirect = &current->next;
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
    if (!head)
        return;

    struct list_head *curr = head, *prev = head->prev, *next = NULL;
    while (next != head) {
        next = curr->next;
        curr->next = prev;
        curr->prev = next;
        prev = curr;
        curr = next;
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
struct list_head *merge(struct list_head *left, struct list_head *right)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; left && right; *node = (*node)->next) {
        element_t *q_left = list_entry(left, element_t, list);
        element_t *q_right = list_entry(right, element_t, list);
        node = strcmp(q_left->value, q_right->value) < 0 ? &left : &right;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) left | (uintptr_t) right);

    return head;
}

struct list_head *MergeSort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast = head->next, *slow = head;
    for (; fast && fast->next; fast = fast->next->next)
        slow = slow->next;
    struct list_head *next = slow->next;
    slow->next = NULL;

    return merge(MergeSort(head), MergeSort(next));
}

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = MergeSort(head->next);

    struct list_head *curr, *prev = head;
    for (curr = head->next; curr; curr = curr->next) {
        curr->prev = prev;
        prev = curr;
    }

    prev->next = head;
    head->prev = prev;
}