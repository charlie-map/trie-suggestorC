#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked.h"
#include "weight.h"

const int MAX_DIST = 3;

typedef struct TrieS {
	int weight;
	int childCount; // keeps track of how many children
	// we have  so we don't have to recalculate
	struct TrieS *children[26];
} Trie;

Trie *childTrie() {
	Trie *childTrie = malloc(sizeof(Trie));

	// loop through children and set them to 0
	for (int i = 0; i < 26; i++) {
		childTrie->children[i] = NULL;
	}

	childTrie->weight = 0;
	childTrie->childCount = 0;

	return childTrie;
}

int insert(Trie *trie, char *value);

int destruct(Trie *trie);

int suggest(Trie *trie, lList *head, char *query, int strPos, char *currentWord, int currentEditDist);

int main() {
	// define head
	Trie *head = childTrie();

	insert(head, "test");
	insert(head, "test");
	insert(head, "es");
	insert(head, "t");
	insert(head, "tes");

	for (int i = 0; i < 26; i++) {
		if (head->children[i])
			printf("Making changes at %d with: %d\n", i, head->children[i]->weight);
	}

	lList *ll_head = makeliNode();

	char *query = "te";
	suggest(head, ll_head, query, -1, "", 0);

	printList(ll_head);

	destruct(head);

	return 0;
}

/*
	INSERT:
		Takes in an input string and will recursively search through
		the trie to add it. The following inputs are required:

		trie: Trie *trie represents the head value of our trie
			insert will be moving down from the head
		value: char *value will be the full user string that
			is passed through the function (this won't change)
		len: int len is the length of the value
		p: our current position in value. as we move down this
			will keep getting added to

		Output:
			The output will just be a register of the ocmpletion of the process,
			but when looking at the trie, it will now have new updated values
*/
int insert(Trie *trie, char *value) {
	int childPoint = (((int) *value) - 97); // find position in children

	// access the trie at that children, if it doesn't exist,
	// make a new sub trie:
	if (!trie->children[childPoint]) {

		trie->childCount++;
		trie->children[childPoint] = childTrie();
	}

	// check for more characters after our current one, if there aren't
	// any more, then we want to go ahead and add to the weight:
	if (!(*(++value))) {
		trie->children[childPoint]->weight++;
		return 0;
	}

	return insert(trie->children[childPoint], value);
}

/*
	Destruct takes in the Trie *trie HEAD and DFS through to
	free all of the allocated heap memory

	After this function the trie will be fully empty (including the head)
*/
int destruct(Trie *trie) {

	if (!trie->childCount) {
		free(trie);
		return 0;
	}

	for (int i = 0; i < 26; i++) {
		if (trie->children[i])
			destruct(trie->children[i]);
	}

	free(trie);
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
		arr: Ranker **arr will be the growing array of the possible values.
			this will have a struct that holds the string and the weight of that
			string
		arrSize: int *arrSize will keep track of the current size of Ranker **arr, so
			we can ensure we have enough space, otherwise we need to expand arr
		arrPos: int *arrPos will store our current position in Ranker **arr and will
			increase by 1 each time we insert a value into arr
		currentWord: char *currentWord is the word we are currently building as
			we DFS through the trie
		currentEditDist: int currentEditDist -- to save time, we will calculate the
			edit distance as we are traversing our trie instead of recalculating
			the value each time
*/
int suggest(Trie *trie, lList *ll_head, char *query, int strPos, char *currentWord, int currentEditDist) {
	// our first step is our base case, we need to ensure
	// that we aren't creeping towards a dead end
	// this is how we will prune our trie:
	if (currentEditDist >= MAX_DIST) {
		// this was a dead end path, but we can add to our rank array if
		// trie has a weight (meaning currentWord is a real word):

		if (trie->weight)
			insertNodeWeighted(ll_head, currentWord, trie->weight, currentEditDist, weightCmp);

		return 0;
	} else {
		if (trie->weight) // we want to add to suggests in this scenario
			insertNodeWeighted(ll_head, currentWord, trie->weight, currentEditDist, weightCmp);
	}

	// otherwise, we need to continue to search for new values
	// by going through each possible route in our trie:
	char *newWordBuild = (char *) malloc(sizeof(char) * (strPos + 3));

	strcpy(newWordBuild, currentWord);
	*(newWordBuild + (strPos + 2) * sizeof(char)) = '\0';

	int hasAdded = 0;

	for (int i = 0; i < 26; i++) {
		*(newWordBuild + (strPos + 1) * sizeof(char)) = (char) i + 97;

		// evaluate trie->children[i]
		// this must exist for us to continue
		if (!trie->children[i]) {
			if (trie->weight && !hasAdded) { // add something to arr: (ONLY FIRST TIME THIS OCCURS)

				hasAdded = 1;
				insertNodeWeighted(ll_head, currentWord, trie->weight, currentEditDist, weightCmp);
			}

			continue;
		}

		suggest(trie->children[i], ll_head,
			query, strPos + 1,
			newWordBuild,
			currentEditDist + (((int) (query[strPos + 1])) == i) ? 0 : 1);

		free(newWordBuild);
	}

	return 0;
}