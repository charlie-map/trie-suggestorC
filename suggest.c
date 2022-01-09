#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "trie.h"

// weighting on weight and dist of each word
const float PERCENT_WEIGHT = 0.4;
const float PERCENT_DIST = 0.6;

const int MAX_DIST = 3;

int compare(float num1, float num2) {
	return num1 > num2;
}

float calcWeight(int trie_weight, int word_dist) {
	return (trie_weight * PERCENT_WEIGHT) - (word_dist * PERCENT_DIST);
}

int load_trie(Trie *trie, char *filename) {
	FILE *file = fopen(filename, "r");

	if (!file) {
		return 0;
	}

	size_t size = sizeof(char) * 80;
	char *reader = malloc(size);

	int counter = 0;

	while (getline(&reader, &size, file) != -1) {
		char *word = malloc(sizeof(char) * 30);
		double word_freq = 0;

		sscanf(reader, "%s %lf", word, &word_freq);

		insert(trie, word, word_freq, 0);
		counter += word_freq;

		free(word);
	}

	free(reader);
	fclose(file);

	return 0;
}

int suggest(Trie *trie, heap_t *heap, char *query, int query_curr_pos, int query_length, char *curr_word, int dist);

int main() {
	// define head
	Trie *head = childTrie();
	heap_t *heap = heap_create(compare);

	load_trie(head, "norvigclean.txt");

	char *start = malloc(sizeof(char));
	start[0] = '\0';
	suggest(head, heap, "hen", 0, 3, start, 0);

	free(start);

	char *option1 = (char *) heap_peek(heap);
	printf("best option %s\n", option1);
	// free(option1);
	// printf("second %s\n", (char *) heap_peek(heap));

	destruct(head);
	heap_destroy(&heap);

	return 0;
}

int destroy(void *payload) {
	free(payload);

	return 0;
}

int suggest(Trie *trie, heap_t *heap, char *query, int query_curr_pos, int query_length, char *curr_word, int dist) {
	// ensure the dist is reasonable (base case)
	if (MAX_DIST <= dist)
		return 0;

	// go through the trie possible children
	// recursively test pathes that are strong
	for (int try_path = 0; try_path < 26; try_path++) {
		// update curr_word and dist on recursive call
		if (trie->children[try_path]) {
			// create a new word and copy over curr_word
			char *new_word = malloc(sizeof(char) * (query_curr_pos + 2));
			strcpy(new_word, curr_word);

			// add the new character
			new_word[query_curr_pos] = (char) 97 + try_path;
			new_word[query_curr_pos + 1] = '\0';

			// calculate new distance
			int new_dist = dist + ((query_curr_pos >= query_length || (int) query[query_curr_pos] == 97 + try_path) ? 0 : 1);
			int pushed = 0;

			// only push to heap if there is a weight at this word
			if (trie->children[try_path]->weight) {
				// alter distance if the length of the query word is longer than the new_word
				int new_word_length = strlen(new_word);

				int weighted_dist = new_dist + (new_word_length < query_length ? query_length - new_word_length : 0);

				// calculate weight of word
				float weight = calcWeight(trie->children[try_path]->weight, weighted_dist);

				// push into the heap
				heap_push(heap, new_word, weight, destroy);
				pushed = 1;
			}

			// recur
			suggest(trie->children[try_path], heap, query, query_curr_pos + 1, query_length, new_word, new_dist);
		
			if (!pushed)
				free(new_word);
		}
	}

	return 0;
}