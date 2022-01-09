#ifndef __TRIE_T__
#define __TRIE_T__

typedef struct TrieS {
	double weight;
	int childCount; // keeps track of how many children
	// we have  so we don't have to recalculate
	struct TrieS *children[26];
} Trie;

Trie *childTrie();

int insert(Trie *trie, char *value, double value_weight, int depth);

int destruct(Trie *trie);

#endif