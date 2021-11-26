#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked.h"
#include "trie.h"

const int MAX_DIST = 3;

const float PER_WEIGHT = 0.6;
const float PER_DIST = 0.4;

typedef struct ll_payload {
	int weight;
	int dist;
	char *word;
} ll_load;

ll_load *ll_makePayload(char *word, int weight, int dist) {
	ll_load *ll_p = (ll_load*) malloc(sizeof(ll_load));

	ll_p->weight = weight;
	ll_p->dist = dist;
	if (word) {
		ll_p->word = (char *) malloc(sizeof(word));
		strcpy(ll_p->word, word);
	}

	return ll_p;
}

void ll_payloadDestroy(void *payload) {
	free(((ll_load *) payload)->word);
	free(payload);

	return;
}

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
float weightCmp(void *cmp1, void *cmp2) {
	int cmp1_weight = ((ll_load *) ((ll_main *)cmp1)->payload)->weight;
	int cmp1_dist = ((ll_load *) ((ll_main *)cmp1)->payload)->dist;

	int cmp2_weight = ((ll_load *) ((ll_main *)cmp2)->payload)->weight;
	int cmp2_dist = ((ll_load *) ((ll_main *)cmp2)->payload)->dist;

	float weight_dir = (cmp1_weight - cmp2_weight) * PER_WEIGHT;
	float dist_dir = (cmp1_dist - cmp2_dist) * PER_DIST;

	printf("calculated values for %s and %s and got %1.3f %1.3f\n",  ((ll_load *) ((ll_main *)cmp1)->payload)->word, ((ll_load *) ((ll_main *)cmp1)->payload)->word, weight_dir, dist_dir);
	return weight_dir + dist_dir;
}

int suggest(Trie *trie, ll_main **ll_head, char *query, int strPos, char *currentWord, int currentEditDist);

int printList(ll_main *start);

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

 	ll_load *ll_headValues = (ll_load *) ll_makePayload(NULL, 0, 0);
	ll_main *ll_head = makeliNode(ll_headValues);

	char *query = "te";
	char *buildWord = "a";
	suggest(head, &ll_head, query, 0, buildWord, 0);

	printList(ll_head);
	ll_destroy(ll_head, ll_payloadDestroy);

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
int suggest(Trie *trie, ll_main **ll_head, char *query, int strPos, char currentWord[], int currentEditDist) {
	// our first step is our base case, we need to ensure
	// that we aren't creeping towards a dead end
	// this is how we will prune our trie:
	if (trie->weight) {
		printf("adding pre path? %s\n", currentWord);
		insertNodeWeighted(ll_head, ll_makePayload(currentWord, trie->weight, currentEditDist), weightCmp);

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

int printList(ll_main *start) {
	int pos = 0;

	ll_load *buffer = (ll_load *) start->payload;

	while (start->tail) {
		printf("Values for node %d are weight: %d and edit distance: %d, with a word: %s\n", pos, buffer->weight, buffer->dist, buffer->word);
		start = start->tail;
		pos++;
	}

	printf("Values for node %d are weight: %d and edit distance: %d, with a word: %s\n", pos, buffer->weight, buffer->dist, buffer->word);
	return 0;
}