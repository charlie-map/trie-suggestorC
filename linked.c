#include <stdio.h>
#include <stdlib.h>
#include "linked.h"

lList *makeliNode() {
	lList *pumperNode = malloc(sizeof(lList));

	pumperNode->tail = NULL;
	pumperNode->head = NULL;
	pumperNode->weight = 0;
	pumperNode->dist = 0;

	return pumperNode;
}

int insertNodeWeighted(lList *ll_runner, char *word, int weight, int dist, float (*comparer)(lList *old, lList *new)) {
	lList *newNode = makeliNode();

	newNode->weight = weight;
	newNode->dist = dist;
	newNode->word = word;

	while (ll_runner->tail && comparer(ll_runner, newNode) > 0) {
		ll_runner = ll_runner->tail;
	}

	// insert wherever we currently are
	// save the ll_runner's current tail:
	lList *buffer = ll_runner->tail;
	ll_runner->tail = newNode;
	newNode->tail = buffer;

	return 0;
}

/*
	The following are basic functions that may be
	used. Creating some of them more for fun.
*/
int insertLast(lList *node, int weight, int dist) {

	while (node->tail)
		node = node->tail;

	node->tail = (lList *) makeliNode();
	node->tail->head = node;
	node->tail->weight = weight;
	node->tail->dist = dist;

	return 0;
}

int insertFirst(lList *node, int weight, int dist) {
	lList *new = makeliNode();

	new->tail = node;
	node->head = new;
	new->weight = weight;
	new->dist = dist;

	return 0;
}

/*
	Reverse:
		Will reverse every item in the linked list
		from start to the end

	Note: starts from front!
*/
lList *reverse(lList *curr) {
	lList *prev = NULL;
	lList *next = curr->tail;

	while (next) {
		curr->tail = prev;
		prev = curr;

		curr = next;
		next = next->tail;
	}

	curr->tail = prev;

	return curr;
}

int printList(lList *start) {
	int pos = 0;

	while (start->tail) {
		printf("Values for node %d are weight: %d and edit distance: %d, with a word: %s\n", pos, start->weight, start->dist, start->word);
		start = start->tail;
		pos++;
	}

	printf("Values for node %d are weight: %d and edit distance: %d, with a word: %d\n", pos, start->weight, start->dist, start->word);
	return 0;
}

int destroy(lList *start) {

	if (start->tail)
		destroy(start->tail);

	free(start);

	return 0;
}