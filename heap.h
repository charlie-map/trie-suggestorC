#ifndef __HEAP_T__
#define __HEAP_T__

typedef struct MinPointer heap_t;

heap_t *heap_create(int (*compare)(float, float));

void *heap_push(heap_t *min, void *payload, float weight, int (*destroy)(void *));
void *heap_peek(heap_t *head);
void *heap_pop(heap_t *head);

int heap_decrease_key(heap_t *head, void *node, float newWeight);

int heap_destroy(heap_t **head);

#endif