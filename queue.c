#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *pos, *n = NULL;
    // list_for_each_safe:
    // iterate over a list safe against removal of list entry
    list_for_each_safe (pos, n, l) {
        // free the currenct postion
        free(pos);
    }
    // after removing all nodes in queue, free head pointer
    free(l);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    // malloc a spcae for newnode
    if (!head)
        return false;
    element_t *newnode = malloc(sizeof(element_t));
    if (!newnode) {
        free(newnode);
        return false;
    }

    // +1 for '/0'
    char *str_ptr = malloc(sizeof(char) * strlen(s) + 1);
    if (!str_ptr) {
        free(str_ptr);
        free(newnode);
        return false;
    }

    memcpy(str_ptr, s, strlen(s) + 1);
    newnode->value = str_ptr;
    list_add(&newnode->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    // similar to insert_head
    element_t *newnode = malloc(sizeof(element_t));
    if (!newnode) {
        free(newnode);
        return false;
    }

    // +1 for '/0'
    char *str_ptr = malloc(sizeof(char) * strlen(s) + 1);
    if (!str_ptr) {
        free(str_ptr);
        free(newnode);
        return false;
    }

    memcpy(str_ptr, s, strlen(s) + 1);
    newnode->value = str_ptr;
    list_add_tail(&newnode->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // copy remove node's value into sp, and limit sp size < bufsize
    if (!head || list_empty(head))
        return NULL;
    element_t *del_node = list_first_entry(head, element_t, list);
    list_del(&del_node->list);
    if (sp) {
        // when bufsize>1, sp can be assinged with char.
        for (char *str_ptr = del_node->value; bufsize > 1;
             sp++, str_ptr++, bufsize--)
            *sp = *str_ptr;
        *sp = '\0';  // end of string ends with \0
    }
    return del_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // /**
    //  * since each node has prev and next pointer,
    //  * we can simply find tail position in queue in O(1)
    //  */
    if (!head || list_empty(head))
        return NULL;
    element_t *del_node =
        list_entry(head->prev, element_t, list);  // find entry for remove node
    if (!del_node)
        return NULL;
    list_del(&del_node->list);
    if (sp) {
        // when bufsize>1, sp can be assinged with char.
        for (char *str_ptr = del_node->value; bufsize > 1;
             sp++, str_ptr++, bufsize--)
            *sp = *str_ptr;
        *sp = '\0';  // end of string ends with \0
    }
    return del_node;
}

/* Return number of elements in queue */
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

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head)
        return false;

    struct list_head *fast = head->next->next;
    struct list_head *slow = head->next;

    // after while loop, slow == mid_position
    while (fast->prev != head && fast != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    // update queue
    list_del(slow);
    element_t *rmv_ptr = list_entry(slow, element_t, list);
    q_release_element(rmv_ptr);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // note:
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
