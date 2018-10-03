#include "Heap.h"
#include <stdlib.h>

void* findMin(Heap *heap)
{
	return heap->data[0].data;
}

void createHeap(Heap *heap, int size, int(*comparator)(void *, void *))
{
	heap->data = (Heap_Node*) malloc(sizeof(Heap_Node) * size);
	heap->maxSize = size;
	heap->comparator = comparator;
	heap->size = 0;
}


void heapifyUp(Heap *heap, unsigned int index)
{
	int current = index;
	void *temp;
	int parent;
	while (1)
	{
		if (current == 0)
			break;

		parent = (current - 1) / 2;
		if (heap->comparator(heap->data[current].data, heap->data[parent].data) == 0)
		{
			temp = heap->data[current].data;
			heap->data[current].data = heap->data[parent].data;
			heap->data[parent].data = temp;
			current = parent;
		}
		else
			break;
	}
}


void heapifyDown(Heap *heap, unsigned int index)
{
	int current = index;
	void *temp;
	int child1;
	int child2;
	int swap;
	while (1)
	{
		if (current >= heap->size)
			break;

		child1 = current * 2 + 1;
		child2 = current * 2 + 2;



		if (heap->comparator(heap->data[current].data, heap->data[child1].data) == 1 || heap->comparator(heap->data[current].data, heap->data[child2].data) == 1)
		{
			if (heap->comparator(heap->data[child1].data, heap->data[child2].data) == 1)
				swap = child1;
			else
				swap = child2;


			temp = heap->data[current].data;
			heap->data[current].data = heap->data[swap].data;
			heap->data[swap].data = temp;
			current = swap;
		}
		else
			break;
	}
}

void* pop(Heap *heap)
{
	void *result = heap->data[0].data;
	heap->data[0].data = heap->data[heap->size - 1].data;
	heap->size--;
	heapifyDown(heap, 0);

	return result;
}

int push(Heap *heap, void* item)
{
	if (heap->size == heap->maxSize)
	{
		return 0;
	}
	heap->data[heap->size].data = item;
	heap->size++;
	heapifyUp(heap, heap->size - 1);

	return 1;
}

void* replace(Heap *heap, void* item)
{
	void *result = heap->data[0].data;
	heap->data[0].data = item;
	heapifyDown(heap, 0);

	return result;
}
