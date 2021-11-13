#ifndef __LINKED_T__
#define __LINKED_T__

typedef struct List {
	struct List *tail;
	struct List *head;
	int weight; // occurence of word
	int dist; // distance from user input
	char *word;
} lList;

lList *makeliNode();

int insertNodeWeighted(lList *ll_runner, int weight, int dist, float (*comparer)(lList *old, lList *new));

int insertLast(lList *node, int weight, int dist);

int insertFirst(lList *node, int weight, int dist);

lList *reverse(lList *start);

int printList(lList *start);

int destroy(lList *start);

#endif