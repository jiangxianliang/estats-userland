#if !defined(ESTATS_LIST_INT_H)
#define ESTATS_LIST_INT_H

/*
 * The below code is heavily based off of the Linux kernel's list.h.
 */

static inline void _estats_list_init(struct estats_list* ptr)
{
    ptr->next = ptr;
    ptr->prev = ptr;
}

static inline void _estats_list_add2(struct estats_list* new,
                                     struct estats_list* prev,
                                     struct estats_list* next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void _estats_list_add(struct estats_list* new,
                                    struct estats_list* head)
{
    _estats_list_add2(new, head, head->next);
}

static inline void _estats_list_add_tail(struct estats_list* new,
                                         struct estats_list* head)
{
    _estats_list_add2(new, head->prev, head);
}

static inline void _estats_list_del2(struct estats_list* prev,
                                     struct estats_list* next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void _estats_list_del(struct estats_list* entry)
{
    _estats_list_del2(entry->prev, entry->next);
    entry->prev = entry->next = 0;
}

static inline int _estats_list_empty(struct estats_list* head)
{
    return head->next == head;
}

#define ESTATS_LIST_ENTRY(ptr, type, member) \
    ((type*)((char*)(ptr) - (unsigned long)(&((type*)0)->member)))
#define ESTATS_LIST_FOREACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
#define ESTATS_LIST_FOREACH_PREV(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)
#define ESTATS_LIST_FOREACH_SAFE(pos, tmp, head) \
    for (pos = (head)->next, tmp = pos->next; pos != (head); pos = tmp, tmp = pos->next)

#endif /* !defined(ESTATS_LIST_INT_H) */
