#ifndef __LINKED_T__
#define __LINKED_T__

typedef struct List {
	struct List *tail;
	void *payload;
} ll_main;

ll_main *makeliNode();

int insertNodeWeighted(ll_main **ll_runner, void *payload, float (*comparer)(void *, void *));

int insertLast(ll_main *node, void *payload);

ll_main *insertFirst(ll_main *node, void *payload);

ll_main *reverse(ll_main *start);

int ll_destroy(ll_main *start, void (*ll_payloadDestroy)(void *));

#endif