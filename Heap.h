#ifndef HEAP_H_
#define HEAP_H_

typedef struct
{
	void *data;
}HeapNode;

class Heap
{
	private:
		

	public:
		HeapNode* data;
		int maxSize;
		int size;
		int(*comparator)(void*, void*);
		
		Heap(int size, int(*comparators)(void*, void*));
		
		void* pop();
		int push(void* item);
		void* replace(void* item);
		void* peak();
		void heapifyUp(unsigned int index);
		void heapifyDown(unsigned int index);
};

#endif