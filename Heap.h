#ifndef HEAP_H_

#include <stdlib.h>

typedef struct
{
	void *data;
}Heap_Node;


typedef struct
{
	Heap_Node* data;
	int maxSize;
	int size;

	int(*comparator)(void *, void *);
}Heap;


void createHeap(Heap *heap, int size, int(*comparator)(void *, void *));
void heapifyUp(Heap *heap, unsigned int index);
void heapifyDown(Heap *heap, unsigned int index);
void* pop(Heap *heap);
int push(Heap *heap, void* item);
void* replace(Heap *heap, void* item);
void* peak(Heap *heap);
void printHeap(Heap *heap);

#endif // !HEAP_H_
