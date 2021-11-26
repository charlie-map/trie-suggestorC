#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked.h"

const int WEIGHT_NODE_MAX = 10;

ll_main *makeliNode(void *payload) {
	ll_main *pumperNode = malloc(sizeof(ll_main));

	pumperNode->payload = payload;

	return pumperNode;
}

int insertNodeWeighted(ll_main **ll_runner, void *payload, float (*comparer)(void *, void *)) {
	ll_main *newNode = makeliNode(payload);

	if (!(*ll_runner)->tail)
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
	ll_main *buffer = (*ll_runner)->tail;
	(*ll_runner)->tail = newNode;
	newNode->tail = buffer;

	return 0;
}

/*
	The following are basic functions that may be
	used. Creating some of them more for fun.
*/
int insertLast(ll_main *node, void *payload) {

	while (node->tail)
		node = node->tail;

	node->tail = (ll_main *) makeliNode(payload);

	return 0;
}

ll_main *insertFirst(ll_main *node, void *payload) {
	ll_main *new = makeliNode(payload);

	new->tail = node;

	return new;
}

/*
	Reverse:
		Will reverse every item in the linked list
		from start to the end

	Note: starts from front!
*/
ll_main *reverse(ll_main *curr) {
	ll_main *prev = NULL;
	ll_main *next = curr->tail;

	while (next) {
		curr->tail = prev;
		prev = curr;

		curr = next;
		next = next->tail;
	}

	curr->tail = prev;

	return curr;
}

int ll_destroy(ll_main *start, void (*ll_payloadDestroy)(void *)) {

	if (start->tail)
		ll_destroy(start->tail, ll_payloadDestroy);

	if (ll_payloadDestroy)
		ll_payloadDestroy(start->payload);
	free(start);

	return 0;
}