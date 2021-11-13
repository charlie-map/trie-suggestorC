#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

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