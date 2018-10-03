
#include <stdlib.h>
#include <stdio.h>
#include "Heap.h"
#include "main.h"
#include "TurnManger.h"


Heap* CreateTurnManager(MapInfo *mapInfo)
{
	Heap* newHeap = (Heap*) malloc(sizeof(Heap));
	createHeap(newHeap, mapInfo->numMonster + 1, ComparePC);
	
	PCEvent* newEvent = (PCEvent*)malloc(sizeof(PCEvent));
	newEvent->Player = &(mapInfo->Player);
	newEvent->Round = 1000 / newEvent->Player->speed;
	push(newHeap, newEvent);

	int i;
	for (i = 0; i < mapInfo->numMonster; i++)
	{
		newEvent = (PCEvent*)malloc(sizeof(PCEvent));
		newEvent->Player = &(mapInfo->Monsters[i]);
		newEvent->Round = 1000 / mapInfo->Monsters[i].speed;
		push(newHeap, newEvent);
	}

	return newHeap;
}


int ComparePC(void* PC1, void*PC2)
{
	PCEvent* A = (PCEvent*)PC1;
	PCEvent* B = (PCEvent*)PC2;
	if (A->Round > B->Round)
		return 1;
	return 0;
}


void NextTurn(MapInfo *mapInfo, Heap* turnManager)
{

}