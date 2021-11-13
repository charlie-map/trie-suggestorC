#include <stdio.h>
#include <stdlib.h>
#include "linked.h"

const float PER_WEIGHT = 0.6;
const float PER_DIST = 0.4;

/*
	weightCmp:
		this function is how we decide the "signifigance"
		of each word

		We will take in two components in our list and
		check which one is more important. If the return value
		of weightCmp is -1, then we know that cmp1 is less
		"signifigant". A value of 1 means that cmp1 is more
		"signifigant"

		The return value will depend on two factors:
			the weight
			the dist

		The weight of each of those is decided by two float constants
		that have a percentage of 1.
*/
float weightCmp(lList *cmp1, lList *cmp2) {
	int cmp1_weight = cmp1->weight;
	int cmp1_dist = cmp1->dist;

	int cmp2_weight = cmp2->weight;
	int cmp2_dist = cmp2->dist;

	float weight_dir = (cmp1_weight - cmp2_weight) * PER_WEIGHT;
	float dist_dir = (cmp1_dist - cmp2_dist) * PER_DIST;

	return weight_dir + dist_dir;
}

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