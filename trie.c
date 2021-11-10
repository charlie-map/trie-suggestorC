#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_DIST = 3;

typedef struct TrieS {
	int weight;
	int childCount; // keeps track of how many children
	// we have  so we don't have to recalculate
	struct TrieS *children[26];
} Trie;

typedef struct Ranks {
	char *word;
	int ranker;
} Ranker;

int insert(Trie *trie, char *value, int len, int p);

int destruct(Trie *trie);

Ranker *suggest(Trie *trie, char *query, int strPos, Ranker **arr, int *arrSize, int *arrPos, char *currentWord, int currentEditDist);

int main() {
	// define head
	Trie *head = malloc(sizeof(Trie));
	head->childCount = 0;

	insert(head, "test", strlen("test"), 0);
	insert(head, "test", strlen("test"), 0);
	insert(head, "es", strlen("es"), 0);
	insert(head, "t", strlen("t"), 0);
	insert(head, "tes", strlen("tes"), 0);

	for (int i = 0; i < 26; i++) {
		if (head->children[i])
			printf("Making changes at %d with: %d\n", i, head->children[i]->weight);
	}

	// define Ranker initial values
	Ranker array[16];
	Ranker *rankerPointer = (Ranker *) array;

	int arrSize = 16;
	int *arrSizePnt = &arrSize;
	int arrPos = 0;
	int *arrPosPnt = &arrPos;

	char *query = "te";
	suggest(head, query, 0, &rankerPointer, arrSizePnt, arrPosPnt, "", 0);

	// loop through and check our head:
	for (int i = 0; i < arrPos + 1; i++) {
		printf("Look at i %d and value there %d and string %d\n", i, array[i].ranker, array[i].word);
		free(array[i].word);
	}

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
int insert(Trie *trie, char *value, int len, int p) {

	int childPoint = (((int) value[p]) - 97); // find position in children

	// access the trie at that children, if it doesn't exist,
	// make a new sub trie:
	if (!trie->children[childPoint]) {
		Trie *childTrie = malloc(sizeof(Trie));

		trie->childCount++;
		trie->children[childPoint] = childTrie;
	}

	// if we reached the end of our input word
	if (p == len - 1) {
		trie->children[childPoint]->weight++;
		return 0;
	}

	return insert(trie->children[childPoint], value, len, p + 1);
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
	rankerAdd does the process of adding a word into arr as well as the weight,
	which will be used for later ranking. This will also take care of if Ranker **arr
	runs out of space
*/
int rankerAdd(Trie *trie, Ranker **arr, int *arrSize, int *arrPos, char *currentWord, int wordLength) {
	if (*arrPos == *arrSize) {
		*arrSize *= 2;

		*arr = (Ranker *) realloc(*arr, *arrSize * (sizeof(Ranker)));
	}

	((*arr) + *arrPos)->ranker = trie->weight;
	((*arr) + *arrPos)->word = malloc(sizeof(char) * wordLength);
	strcpy(((*arr) + *arrPos)->word, currentWord);

	printf("ADDED TO WORD %s to %s\n", currentWord, ((*arr) + *arrPos)->word);

	*arrPos++;

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
Ranker *suggest(Trie *trie, char *query, int strPos, Ranker **arr, int *arrSize, int *arrPos, char *currentWord, int currentEditDist) {
	// our first step is our base case, we need to ensure
	// that we aren't creeping towards a dead end
	// this is how we will prune our trie:
	if (currentEditDist >= MAX_DIST) {
		// this was a dead end path, but we can add to our rank array if
		// trie has a weight (meaning currentWord is a real word):

		if (trie->weight)
			rankerAdd(trie, arr, arrSize, arrPos, currentWord, strPos + 1);

		return *arr;
	}

	printf("Value incoming %d\n", trie->weight);
	printf("Access %d\n", trie->children[0]);

	// otherwise, we need to continue to search for new values
	// by going through each possible route in our trie:
	char newWordBuild[strPos + 2];

	strcpy(newWordBuild, currentWord);

	int hasAdded = 0;

	for (int i = 0; i < 26; i++) {
		newWordBuild[strPos + 1] = i + 97;

		printf("\tadding to buffer size %d and word %s\n", strPos + 1, newWordBuild);

		// evaluate trie->children[i]
		// this must exist for us to continue
		if (!trie->children[i]) {
			if (trie->weight && !hasAdded) { // add something to arr: (ONLY FIRST TIME THIS OCCURS)
				printf("Adding new word %s\n", currentWord);

				hasAdded = 1;
				rankerAdd(trie, arr, arrSize, arrPos, currentWord, strPos + 1);
			}

			continue;
		}

		printf("Rolling forward %d\n", trie->children[i]->weight);

		suggest(trie->children[i],
			query, strPos + 1, arr, arrSize, arrPos,
			newWordBuild,
			currentEditDist + (((int) (query[strPos])) == i) ? 0 : 1);
	}

	return *arr;
}