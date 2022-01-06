#ifndef __SUGGEST_L__
#define __SUGGEST_L__

int suggest(Trie *trie, heap_t *heap, char *query, int query_curr_pos, int query_length, char *curr_word, int dist);

#endif