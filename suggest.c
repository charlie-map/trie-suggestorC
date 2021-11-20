#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked.h"
#include "trie.h"

const int MAX_DIST = 3;

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
	//printf("calculating for %s %s and get")

	float weight_dir = (cmp1_weight - cmp2_weight) * PER_WEIGHT;
	float dist_dir = (cmp1_dist - cmp2_dist) * PER_DIST;

	printf("calculated values for %s and %s and got %1.3f %1.3f\n",  cmp1->word, cmp2->word, weight_dir, dist_dir);
	return weight_dir + dist_dir;
}

int suggest(Trie *trie, lList **ll_head, char *query, int strPos, char *currentWord, int currentEditDist);

int main() {
	// define head
	Trie *head = childTrie();

	insert(head, "test");
	insert(head, "test");
	insert(head, "es");
	insert(head, "t");
	insert(head, "tes");
	insert(head, "teal");
	insert(head, "teal");
	insert(head, "teal");

	lList *ll_head = makeliNode();

	char *query = "te";
	char *buildWord = "a";
	suggest(head, &ll_head, query, 0, buildWord, 0);

	printList(ll_head);
	ll_destroy(ll_head);

	destruct(head);

	return 0;
}

/*
SUGGEST:
	Trie is going to calculate possible suggestions for a given query
	this will work as an initial qualifier to then calculate which words
	are most important.

	trie: Trie *trie points to the head of the trie. There will be a search
		through a sliced portion of the tree
	query: char *query is the string to be finding suggestions for
	strPos: int strPos points to our position in query, so we can use this for
		deciding whether our current number matches our search path in the tree.
		This will also tell us once we're our of range of the query word,
		which is where we then start using forward suggest
	currentWord: char *currentWord is the word we are currently building as
		we DFS through the trie
	currentEditDist: int currentEditDist -- to save time, we will calculate the
		edit distance as we are traversing our trie instead of recalculating
		the value each time
*/
int suggest(Trie *trie, lList **ll_head, char *query, int strPos, char currentWord[], int currentEditDist) {
	// our first step is our base case, we need to ensure
	// that we aren't creeping towards a dead end
	// this is how we will prune our trie:
	if (trie->weight) {
		printf("adding pre path? %s\n", currentWord);
		insertNodeWeighted(ll_head, currentWord, trie->weight, currentEditDist, weightCmp);

		printf("\n\nBIG LL TEST\n");
		printList(*ll_head);
	}

	if (currentEditDist >= MAX_DIST)
		// this was a dead end path
		return 0;

	// otherwise, we need to continue to search for new values
	// by going through each possible route in our trie:
	printf("running through word %s\n", currentWord);
	if (currentWord[strPos] == '\0') {
		printf("%d\n", strPos + 1);
		currentWord = realloc(currentWord, ((strPos + 1) * 2) * sizeof(char));
		currentWord[strPos * 2 + 1] = '\0';
	}

	int hasAdded = 0;

	printf("Going to add to length %d with %d: %c\n", strlen(currentWord), strPos, currentWord[strPos]);
	for (int i = 0; i < 26; i++) {
		printf("%c and %c\n", currentWord[strPos], (char) i + 97);
		currentWord[strPos] = (char) i + 97;
		//currentWord[strPos + 1] = '\0';

		// evaluate trie->children[i]
		// this must exist for us to continue
		if (!trie->children[i]) {

			continue;
		}

		//printf("Continuing? %c for curr word: %s with dist? %d\n", (char) i + 97, currentWord, (((int) (query[strPos + 1])) == i) ? 0 : 1);
		suggest(trie->children[i], ll_head,
			query, strPos + 1,
			currentWord,
			currentEditDist + (((int) (query[strPos])) == i
				|| !(query[strPos])) ? 0 : 1);

		//free(currentWord);
	}

	return 0;
}