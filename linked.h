#ifndef __LINKED_T__
#define __LINKED_T__

typedef struct List {
	struct List *tail;
} lList;

lList *makeliNode();

int insertNodeWeighted(lList **ll_runner, void *payload, float (*comparer)(lList *old, lList *new));

int insertLast(lList *node, void *payload);

int insertFirst(lList *node, void *payload);

lList *reverse(lList *start);

int printList(lList *start);

int ll_destroy(lList *start);

#endif