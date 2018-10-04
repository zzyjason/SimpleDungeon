#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Heap.h"
#include "main.h"
#include "TurnManger.h"
#include "DungeonGenerator.h"
#include <string.h>
#include <limits.h>


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


unsigned int NextTurn(MapInfo *mapInfo, Heap* turnManager)
{
	PCEvent* Next = (PCEvent *)peak(turnManager);

	while (Next->Player->status == 0)
	{
		pop(turnManager);
		Next = (PCEvent *)peak(turnManager);

		if (Next == NULL)
			return 0;
	}

	unsigned int turn = Next->Round;

	if (PCAction(mapInfo, Next->Player) == -1)
		return 0;

	Next->Round = turn + 1000 / Next->Player->speed;
	heapifyDown(turnManager, 0);

	if (Next->Player->PCType == 16)
	{
		printHallway(mapInfo);
		usleep(500000);
	}


	return turn;
}

int PCAction(MapInfo *mapInfo, PC *Player)
{
	Point Now;
	Point NextMove;

	Now.x = Player->Position.x;
	Now.y = Player->Position.y;

	NextMove.x = Now.x;
	NextMove.y = Now.y;

	int random;

	if (Player->PCType == 16)
		return 0;

	if (Player->PCType & 0b00100 || CheckOfSight(mapInfo, Player->Position))
	{
		Player->LastKnownPC.x = mapInfo->Player.Position.x;
		Player->LastKnownPC.y = mapInfo->Player.Position.y;
	}
	else
		random = 1;
	
	if (Player->PCType & 0b01000)
	{
		if (Player->LastKnownPC.x == 255 || Player->LastKnownPC.y == 255)
			random = 1;
		else 
		{
			if (Player->PCType & 0b00010)
			{

				NextMove.x = mapInfo->TunnelPath.ShortestPath[PointToIndex(&Now)].x;
				NextMove.y = mapInfo->TunnelPath.ShortestPath[PointToIndex(&Now)].y;
			}
			else
			{
				NextMove.x = mapInfo->nonTunnelPath.ShortestPath[PointToIndex(&Now)].x;
				NextMove.y = mapInfo->nonTunnelPath.ShortestPath[PointToIndex(&Now)].y;
			}
		}


		if (comparePoint(Player->LastKnownPC, Player->Position))
		{
			Player->LastKnownPC.x = 255;
			Player->LastKnownPC.y = 255;
		}
	}
	else
	{
		if (!CheckOfSight(mapInfo, Player->Position))
		{
			random = 1;
		}
		else
		{
			if (Player->Position.x > mapInfo->Player.Position.x)
				NextMove.x++;
			if (Player->Position.x < mapInfo->Player.Position.x)
				NextMove.x--;


			if (Player->Position.y > mapInfo->Player.Position.y)
				NextMove.y++;
			if (Player->Position.y < mapInfo->Player.Position.y)
				NextMove.y--;
		}
		
	}

	if ((Player->PCType & 0b00001 && rand() % 2) || random)
	{
		
		do
		{
			random = rand() % 8;
			NextMove.x = Player->Position.x;
			NextMove.y = Player->Position.y;

			switch (random)
			{
				case 0:
					NextMove.x++;
					break;

				case 1:
					NextMove.x--;
					break;

				case 2:
					NextMove.y++;
					break;

				case 3:
					NextMove.y--;
					break;

				case 4:
					NextMove.x++;
					NextMove.y++;
					break;

				case 5:
					NextMove.x++;
					NextMove.y--;
					break;

				case 6:
					NextMove.x--;
					NextMove.y++;
					break;

				case 7:
					NextMove.x--;
					NextMove.y--;
					break;

			}

			random = 0;
		} while (mapInfo->map[PointToIndex(&NextMove)] == ' ' && (Player->PCType & 0b00010));
	}


	if (mapInfo->map[PointToIndex(&NextMove)] == ' ')
	{
		if (mapInfo->hardness[PointToIndex(&NextMove)] > 85)
		{
			mapInfo->hardness[PointToIndex(&NextMove)] -= 85;
			return 0;
		}

		mapInfo->hardness[PointToIndex(&NextMove)] = 0;
		mapInfo->mapLayout[PointToIndex(&NextMove)] = '#';

	}

	if (mapInfo->map[PointToIndex(&NextMove)] != '.' && mapInfo->map[PointToIndex(&NextMove)] != '#')
	{
		if (KillPC(mapInfo, NextMove) == -1)
			return -1;
	}

	mapInfo->map[PointToIndex(&NextMove)] = Player->symbol;
	mapInfo->map[PointToIndex(&Now)] = mapInfo->mapLayout[PointToIndex(&Now)];
	Player->Position.x = NextMove.x;
	Player->Position.y = NextMove.y;

	return 0;
}

int CheckOfSight(MapInfo *mapInfo, Point position)
{

	int count = 0;
	Point current;
	current.x = position.x;
	current.y = position.y;
	
	int yMin = INT_MAX;
	int yMax = INT_MAX;

	
	while (mapInfo->map[PointToIndex(&current)] == '.' || mapInfo->map[PointToIndex(&current)] == '#')
	{
		current.x--;

		if (mapInfo->map[PointToIndex(&current)] == '@')
			return 1;

		while ((mapInfo->map[PointToIndex(&current)] == '.' || mapInfo->map[PointToIndex(&current)] == '#') && count <= yMin)
		{
			current.y--;
			if (mapInfo->map[PointToIndex(&current)] == '@')
				return 1;

			count++;
		}
		current.y = position.y;
		yMin = count;
		count = 0;

		while ((mapInfo->map[PointToIndex(&current)] == '.' || mapInfo->map[PointToIndex(&current)] == '#') && count <= yMax)
		{
			current.y++;
			if (mapInfo->map[PointToIndex(&current)] == '@')
				return 1;

			count++;
		}

		if (mapInfo->map[PointToIndex(&current)] == '@')
			return 1;

		current.y -= count;
		yMax = count;
	}

	current.x = position.x;
	current.y = position.y;

	while (mapInfo->map[PointToIndex(&current)] == '.' || mapInfo->map[PointToIndex(&current)] == '#')
	{
		
		while ((mapInfo->map[PointToIndex(&current)] == '.' || mapInfo->map[PointToIndex(&current)] == '#') && count <= yMin)
		{
			current.y--;
			if (mapInfo->map[PointToIndex(&current)] == '@')
				return 1;

			count++;
		}
		current.y = position.y;
		yMin = count;
		count = 0;

		while ((mapInfo->map[PointToIndex(&current)] == '.' || mapInfo->map[PointToIndex(&current)] == '#') && count <= yMax)
		{
			current.y++;
			if (mapInfo->map[PointToIndex(&current)] == '@')
				return 1;

			count++;
		}

		if (mapInfo->map[PointToIndex(&current)] == '@')
			return 1;

		current.y -= count;
		yMax = count;

		current.x++;
		if (mapInfo->map[PointToIndex(&current)] == '@')
			return 1;
	}

	return 0;
}


int KillPC(MapInfo *mapInfo, Point position)
{
	if (comparePoint(mapInfo->Player.Position, position))
		return -1;

	int i;
	 
	for (i = 0; i < mapInfo->numMonster; i++)
	{
		if (comparePoint(position, mapInfo->Monsters[i].Position))
		{
			mapInfo->Monsters[i].status = 0;
			return 0;
		}
	}

	return -2;
}


int comparePoint(Point m, Point n)
{
	if (m.x == n.x && m.y == n.y)
		return 1;

	return 0;
}