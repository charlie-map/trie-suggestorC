#include <stdio.h>
#include <stdlib.h>
#include "linked.h"
#include "weight.h"

int main() {
	lList *head = makeliNode();
	head->weight = 80001;

	insertLast(head, 69, 420);
	insertLast(head, 20, 3);
	insertLast(head, 1, 1);

	printList(head);

	head = reverse(head);

	printf("\nReversed\n\n");

	printList(head);

	insertNodeWeighted(head, 25, 0, weightCmp);

	printf("\nafter weight\n\n");

	printList(head);

	destroy(head);	
}