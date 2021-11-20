#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked.h"

const int WEIGHT_NODE_MAX = 10;

lList *makeliNode(void *payload) {
	lList *pumperNode = malloc(sizeof(lList));

	pumperNode->payload = payload;

	return pumperNode;
}

int insertNodeWeighted(lList **ll_runner, void *payload, float (*comparer)(lList *old, lList *new)) {
	lList *newNode = makeliNode(payload);

	if (!(*ll_runner)->word)
		return 0;

	int posInNode = 0;

	while ((*(ll_runner))->tail && comparer(*ll_runner, newNode) > 0) {
		*ll_runner = (*ll_runner)->tail;

		posInNode++;

		if (posInNode > WEIGHT_NODE_MAX) { // no reason to keep more than 10
			free(newNode);
			return 0;
		}
	}

	// insert wherever we currently are
	// save the ll_runner's current tail:
	lList *buffer = (*ll_runner)->tail;
	(*ll_runner)->tail = newNode;
	newNode->tail = buffer;

	return 0;
}

/*
	The following are basic functions that may be
	used. Creating some of them more for fun.
*/
int insertLast(lList *node, void *payload) {

	while (node->tail)
		node = node->tail;

	node->tail = (lList *) makeliNode(payload);

	return 0;
}

int insertFirst(lList *node, void *payload) {
	lList *new = makeliNode(payload);

	new->tail = node;
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

	printf("Values for node %d are weight: %d and edit distance: %d, with a word: %s\n", pos, start->weight, start->dist, start->word);
	return 0;
}

int ll_destroy(lList *start) {

	if (start->tail)
		ll_destroy(start->tail);

	printf("free word? %s\n", start->word);
	if (start->word)
		free(start->word);
	free(start);

	return 0;
}