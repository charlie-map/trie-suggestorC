#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "trie.h"

const int MAX_DIST = 3;

const float PER_WEIGHT = 0.6;
const float PER_DIST = 0.4;

int compare(float num1, float num2) {
	return num1 < num2;
}

float calcWeight(int trie_weight, int word_dist) {
	return ((word_dist + 1) * PER_DIST) / (trie_weight * PER_WEIGHT);
}

int suggest(Trie *trie, heap_t *heap, char *query, int query_curr_pos, char *curr_word, int dist);

int main() {
	// define head
	Trie *head = childTrie();
	heap_t *heap = heap_create(compare);

	insert(head, "cat");
	insert(head, "act");
	insert(head, "acts");
	insert(head, "acts");

	char *start = malloc(sizeof(char));
	start[0] = '\0';
	suggest(head, heap, "c", 0, start, 0);

	printf("Test %s\n", (char *) heap_peek(heap));

	destruct(head);
	heap_destroy(&heap);

	return 0;
}

int suggest(Trie *trie, heap_t *heap, char *query, int query_curr_pos, char *curr_word, int dist) {
	// ensure the dist is reasonable (base case)

	// go through the trie possible children
	// recursively test pathes that are strong
	for (int try_path = 0; try_path < 26; try_path++) {
		// update curr_word and dist on recursive call
		if (trie->children[try_path]) {
			// create a new word and copy over curr_word
			char *new_word = malloc(sizeof(curr_word) + sizeof(char));
			strcpy(new_word, curr_word);

			// add the new character
			new_word[query_curr_pos + 1] = (char) 97 + try_path;

			// calculate new distance
			int new_dist = dist + ((int) query[query_curr_pos] == 97 + try_path ? 0 : 1);

			// only push to heap if there is a weight at this word
			if (trie->children[try_path]->weight) {

				// calculate weight of word
				printf("check weighting scheme %d and %d\n", trie->children[try_path]->weight, new_dist);
				float weight = calcWeight(trie->children[try_path]->weight, new_dist);

				// push into the heap
				printf("Test new combo %s: %1.3f\n", new_word, weight);
				heap_push(heap, new_word, weight);
			}

			// recur
			suggest(trie->children[try_path], heap, query, query_curr_pos + 1, new_word, new_dist);
		}
	}
	return 0;
}