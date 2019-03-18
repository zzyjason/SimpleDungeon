#include <stdio.h>
#include <stdlib.h>

#include "Heap.h"

Heap::Heap(int size, int(*comparators)(void *, void *))
{
	data = (HeapNode*)malloc(sizeof(HeapNode) * size);
	maxSize = size;
	comparator = comparators;
	size = 0;
}

void Heap::heapifyUp(unsigned int index)
{
	int current = index;
	void *temp;
	int parent;

	while (1)
	{
		if (current == 0)
			break;

		parent = (current - 1) / 2;
		if (comparator(data[current].data, data[parent].data) == 0)
		{
			temp = data[current].data;
			data[current].data = data[parent].data;
			data[parent].data = temp;
			current = parent;
		}
		else
			break;
	}
}

void Heap::heapifyDown(unsigned int index)
{
	int current = index;
	void *temp;
	int child1;
	int child2;
	int swap;

	while (1)
	{
		child1 = current * 2 + 1;
		child2 = current * 2 + 2;

		if (current >= size || (child1 >= size && child2 >= size))
			break;

		if (child1 >= size)
		{

			break;
		}

		if (child2 >= size)
		{
			if (comparator(data[current].data, data[child1].data) == 1)
			{
				swap = child1;
				temp = data[current].data;
				data[current].data = data[swap].data;
				data[swap].data = temp;
				current = swap;
			}
			break;
		}

		if (comparator(data[current].data, data[child1].data) == 1 || comparator(data[current].data, data[child2].data) == 1)
		{
			if (comparator(data[child1].data, data[child2].data) == 1)
				swap = child2;
			else
				swap = child1;


			temp = data[current].data;
			data[current].data = data[swap].data;
			data[swap].data = temp;
			current = swap;
		}
		else
			break;
	}
}


void* Heap::pop()
{
	if (size == 0)
		return NULL;


	void *result = data[0].data;
	data[0].data = data[size - 1].data;
	size--;
	heapifyDown(0);

	return result;
}

int Heap::push(void* item)
{
	if (size == maxSize)
	{
		return 0;
	}
	data[size].data = item;
	size++;
	heapifyUp(size - 1);

	return 1;
}

void* Heap::replace(void* item)
{
	void *result = data[0].data;
	data[0].data = item;
	heapifyDown(0);

	return result;
}


void* Heap::peak()
{
	if (size == 0)
		return NULL;

	return data[0].data;
}

