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
        element_t *free_point = list_entry(pos, element_t, list);
        q_release_element(free_point);
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

    size_t size = strlen(s) + 1;

    char *str_ptr = malloc(sizeof(char) * size);
    if (!str_ptr) {
        free(str_ptr);
        free(newnode);
        return false;
    }

    memcpy(str_ptr, s, size);
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
        strncpy(sp, del_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
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
        strncpy(sp, del_node->value,
                bufsize -
                    1);  // strncpy: control size to copy, avoid buffer overflow
        sp[bufsize - 1] = '\0';
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
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    // 在已經排序的狀況，移走佇列中具備重複內容的節點

    if (!head)
        return false;

    LIST_HEAD(trash);
    element_t *it, *safe;
    struct list_head *cut = head;

    // list_splice: move a node to another list
    // list_cut_postion:
    // e.g. list_origin: head->1->2->3->4 ....
    //      list_target: head
    // cut point = 3
    // after cut
    // origin: head -> 4 -> ...
    // target: head -> 1 -> 2 -> 3 -> head
    list_for_each_entry_safe (it, safe, head,
                              list) {  // safe == curr_node->next
        if (&safe->list != head &&
            strcmp(safe->value, it->value) ==
                0)  // compare curr value with next value, if equals, delete all
                    // of them
            continue;
        /* Detect duplicated elements */
        if (it->list.prev != cut) {
            LIST_HEAD(tmp);
            list_cut_position(&tmp, cut,
                              &it->list);  // (head_to, head_from, node)
            list_splice(&tmp, &trash);
        }
        cut = safe->list.prev;
    }

    /* empty trash */
    list_for_each_entry_safe (it, safe, &trash, list)
        q_release_element(it);

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    // void list_add(*node, *target): insert node after target
    // void list_del(*node): remove target node form list

    struct list_head *n = head->next;
    struct list_head *t = NULL;
    while (n != head && n->next != head) {
        t = n;
        list_move(n, t->next);
        n = n->next;
    }

    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *it, *safe;
    list_for_each_safe (it, safe, head)
        list_move(it, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || k <= 0)
        return;

    int count = 0;
    struct list_head *cur, *next = NULL;

    list_for_each_safe (cur, next, head) {
        count++;
        if (count % k == 0) {
            count--;
            struct list_head *tmp = cur->prev;
            struct list_head *tmp_prev;
            while ((count--) > 0) {
                tmp_prev = tmp->prev;
                list_del(tmp);
                list_add(tmp, cur);

                cur = cur->next;
                tmp = tmp_prev;
            }
            count = 0;
        }
    }
}


/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    // ref: https://hackmd.io/@yanjiew/linux2023q1-lab0#q_sort
    /* Try to use merge sort*/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* Find middle point */
    struct list_head *mid;
    {
        struct list_head *left, *right;
        left = head->next;   // lefest point
        right = head->prev;  // rightest point

        while (left != right && left->next != right) {
            left = left->next;
            right = right->prev;
        }
        mid = left;  // find mid
    }

    /* Divide into two part */
    LIST_HEAD(left);
    LIST_HEAD(right);

    list_cut_position(&left, head, mid);
    list_splice_init(head, &right);

    /* Conquer */
    q_sort(&left);
    q_sort(&right);

    /* Merge */
    while (!list_empty(&left) && !list_empty(&right)) {
        if (strcmp(list_first_entry(&left, element_t, list)->value,
                   list_first_entry(&right, element_t, list)->value) <= 0) {
            list_move_tail(left.next,
                           head);  // list_move_tail: del node and add to head
        } else {
            list_move_tail(right.next, head);
        }
    }

    list_splice_tail(
        &left, head);  // in case left or right is empty, splice the other side.
    list_splice_tail(&right, head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    // if (!head || list_empty(head))
    //     return 0;

    // struct list_head *node, *safe;
    // char *max = list_entry(head->prev, element_t, list)->value;
    // for(node= head->prev, safe = node->prev; node!=head; node =safe,
    // safe=node->prev) { // reverse travesel
    //     element_t *cmp_node = list_entry(node,element_t,list);
    //     if(node != head->prev){
    //         if (strcmp(max,cmp_node->value)<0)
    //             max = cmp_node->value;

    //         list_del(&cmp_node->list);
    //         q_release_element(cmp_node);
    //     }
    // }

    // return q_size(head);
    if (!head || list_empty(head))
        return 0;

    int cnt = 1;
    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_last_entry(&cur->list, element_t, list);
        if (strcmp(prev->value, cur->value) < 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            cnt++;
            cur = prev;
        }
    }

    return cnt;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head)
        return 0;

    LIST_HEAD(tmp);
    queue_contex_t *it;
    /**
     * The macro, list_for_each_entry, exists,
     * but cppcheck tells me it is unknown
     */
    // cppcheck-suppress unknownMacro
    list_for_each_entry (it, head, chain)
        list_splice_init(it->q, &tmp);

    int size = q_size(&tmp);
    q_sort(&tmp);
    list_splice(&tmp, list_first_entry(head, queue_contex_t, chain)->q);

    return size;
}
