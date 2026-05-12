#ifndef _MYLIB_LIST_H
#define _MYLIB_LIST_H

#include <stddef.h>


struct list_node {
    struct list_node *prev;
    struct list_node *next;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define DEFINE_LIST_HEAD(name) \
    struct list_node name = LIST_HEAD_INIT(name)

static inline void list_init(struct list_node *head)
{
    head->next=head;
	head->prev=head;
}

static inline void __list_add(struct list_node *newn,
							  struct list_node *prev,
							  struct list_node *next)
{
    newn->next=next;
	newn->prev=prev;
	prev->next=newn;
	next->prev=newn;
}

static inline void list_add(struct list_node *newn, struct list_node *head)
{
    __list_add(newn,head,head->next);
}

static inline void list_add_tail(struct list_node *newn, struct list_node *head)
{
    __list_add(newn,head->prev,head);
}

static inline void __list_del(struct list_node *prev, struct list_node *next)
{
	prev->next=next;
	next->prev=prev;
}

static inline void list_del(struct list_node *entry)
{
    __list_del(entry->prev,entry->next);
    entry->next=NULL;
    entry->prev=NULL;
}

static inline void list_del_init(struct list_node *entry)
{
    __list_del(entry->prev,entry->next);
    list_init(entry);
}

/* Move a list node to the beginning of the list */
static inline void list_move_head(struct list_node *list, struct list_node *head)
{
    __list_del(list->prev,list->next);
    list_add(list,head);
}

static inline void list_move_tail(struct list_node *list, struct list_node *head)
{
    __list_del(list->prev,list->next);
    list_add_tail(list,head);
}


/* Get the bytes offset of a member in a struct */
#define offset_of(type, member) ((size_t) &((type *)0)->member)

/* Get the container of a struct given its member and pointer to the member 
 * and the type of the container struct. This is a common technique in C to achieve
 * a form of inheritance or to embed a list node within a larger struct. 
 * The macro calculates the address of the container struct by subtracting the
 *  offset of the member from the pointer to the member.
 * 
 * @ptr: the pointer to the member
 * @type: the type of the container
 * @member: the name of the member
 */
#define container_of(ptr, type, member) ( (type *)((char *)(ptr) - offset_of(type, member)) )            

/* Get the c*/
#define list_entry(ptr, type, member) container_of(ptr, type, member)


#define list_for_each(ptr_entry, head) \
    for (ptr_entry = (head)->next; ptr_entry != (head); ptr_entry = ptr_entry->next)

/* Iterate through the list safely, using a temporary pointer */
#define list_for_each_safe(entry, tmp, head) \
    for (entry = (head)->next, tmp = (entry)->next; entry != (head); entry = tmp, tmp = (entry)->next) 


#endif /* _MYLIB_LIST_H */