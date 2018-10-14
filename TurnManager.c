#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Heap.h"
#include "main.h"
#include "TurnManager.h"
#include "DungeonGenerator.h"
#include <string.h>
#include <limits.h>
#include <ncurses.h>
#include "Path.h"
#include "main.h"


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


int NextTurn(MapInfo *mapInfo, Heap* turnManager)
{
	PCEvent* Next = (PCEvent *)peak(turnManager);

	while (Next->Player->status == 0)
	{
		pop(turnManager);
		Next = (PCEvent *)peak(turnManager);

		if (Next == NULL)
			return 0;
	}

	int turn = Next->Round;


	if (Next->Player->PCType == 16)
	{
		printHallway(mapInfo);

		refresh();
	}

	int result = PCAction(mapInfo, Next->Player);
	if (result < 0)
		return result;

	if (result == 3 || result == 4)
	{
		return 0;
	}

	Next->Round = turn + 1000 / Next->Player->speed;
	heapifyDown(turnManager, 0);

	return turn;
}

int UserAction(MapInfo *mapInfo, PC *Player)
{
	flushinp();
	char input;
	Point nextMove;
	WINDOW *info = NULL;

	do
	{
		nextMove.x = 255;
		nextMove.y = 255;
		noecho();
		input = getch();


		if (input == '7' || input == 'y')
		{
			nextMove.x = Player->Position.x - 1;
			nextMove.y = Player->Position.y - 1;
		}
		else if (input == '8' || input == 'k')
		{
			nextMove.x = Player->Position.x;
			nextMove.y = Player->Position.y - 1;
		}
		else if (input == '9' || input == 'u')
		{
			nextMove.x = Player->Position.x + 1;
			nextMove.y = Player->Position.y - 1;
		}
		else if (input == '6' || input == 'l')
		{
			nextMove.x = Player->Position.x + 1;
			nextMove.y = Player->Position.y;
		}
		else if (input == '3' || input == 'n')
		{
			nextMove.x = Player->Position.x + 1;
			nextMove.y = Player->Position.y + 1;
		}
		else if (input == '2' || input == 'j')
		{
			nextMove.x = Player->Position.x;
			nextMove.y = Player->Position.y + 1;
		}
		else if (input == '1' || input == 'b')
		{
			nextMove.x = Player->Position.x - 1;
			nextMove.y = Player->Position.y + 1;

		}
		else if (input == '4' || input == 'h')
		{
			nextMove.x = Player->Position.x - 1;
			nextMove.y = Player->Position.y;

		}
		else if (input == '5' || input == ' ')
		{
			nextMove.x = Player->Position.x;
			nextMove.y = Player->Position.y;

		}
		else if (input == '>')
		{
			if(mapInfo->mapLayout[PointToIndex(&(Player->Position))] == '>')
				return 3;
		}
		else if (input == '<')
		{
			if (mapInfo->mapLayout[PointToIndex(&(Player->Position))] == '<')
				return 4;
		}
		else if (input == 'm')
		{
			if (info == NULL)
			{
				info = newwin(15, 60, 3, 10);
				wborder(info, '|', '|', '-', '-', '+', '+', '+', '+');
				DisplayMonsterInfo(mapInfo, info, 0);
			}
		}
		else if (input == 27)
		{
			if (info != NULL)
			{
				nodelay(info, 1);
				wgetch(info);
				switch (wgetch(info))
				{
					case 'A':
						DisplayMonsterInfo(mapInfo, info, -1);

						break;

					case 'B':

						DisplayMonsterInfo(mapInfo, info, 1);

						break;

					case ERR:

						delwin(info);
						refresh();
						info = NULL;
						printHallway(mapInfo);
						break;
				}
				nodelay(info, 0);
				flushinp();
			}
		}
		else if (input == 'q')
		{
			return -2;
		}

	} while (!MovePC(mapInfo, Player, nextMove));

	UpdatePath(mapInfo);

	return 0;
}
void DisplayMonsterInfo(MapInfo *mapInfo, WINDOW *info, int offset)
{
	static int currentIndex = 0;
	int i, counter, monster = 0, total = 0;
	
	for (i = 0; i < mapInfo->numMonster; i++)
	{
		if (mapInfo->Monsters[i].status == 1)
		{
			total++;
			if (i >= currentIndex)
				monster++;
		}
	}

	switch (offset)
	{
		case 1:
			if (monster - 6 > 0)
				currentIndex++;
			break;
		case -1:
			if (currentIndex != 0)
				currentIndex--;
			break;
	}


	wmove(info, 1, 2);
	wprintw(info, "There is %d Monsters. Use Up/Down Arrow to Scroll", total);

	int xOffset, yOffset, skip=0;
	char dir1[5], dir2[6];

	for (i = 0, counter = 0; i < mapInfo->numMonster && counter < 12; i++)
	{
		if (mapInfo->Monsters[i].status == 0)
			continue;

		if (skip < currentIndex)
		{
			skip++;
			continue;
		}
		
		xOffset = mapInfo->Player.Position.x - mapInfo->Monsters[i].Position.x;
		yOffset = mapInfo->Player.Position.x - mapInfo->Monsters[i].Position.y;

		wmove(info, ++counter + 1, 2);
		
		if (xOffset >= 0)
		{
			strcpy(dir1, "West");
		}
		else
		{
			strcpy(dir1, "East");
		}

		if (yOffset >= 0)
		{
			strcpy(dir2, "South");
		}
		else
		{
			strcpy(dir2, "North");
		}

		wprintw(info, "Monster %c: Location %s %d	%s %d", mapInfo->Monsters[i].symbol, dir1, abs(xOffset), dir2, abs(yOffset));
		counter++;
	}

	wrefresh(info);


}

int MovePC(MapInfo *mapInfo, PC *Player, Point nextMove)
{
	int flag = ValidateNextMove(mapInfo, nextMove, Player->PCType & 0b0010);
	
	if (flag == 0)
		return 0;

	if (flag == 1)
	{
		mapInfo->hardness[PointToIndex(&nextMove)] = 0;

		if (mapInfo->mapLayout[PointToIndex(&(nextMove))] == ' ')
			mapInfo->mapLayout[PointToIndex(&(nextMove))] = '#';
		

		if (mapInfo->map[PointToIndex(&(nextMove))] != mapInfo->mapLayout[PointToIndex(&(nextMove))])
			if (KillPC(mapInfo, nextMove) == -1)
				return -1;

		mapInfo->map[PointToIndex(&(Player->Position))] = mapInfo->mapLayout[PointToIndex(&(Player->Position))];
		mapInfo->map[PointToIndex(&(nextMove))] = Player->symbol;
		Player->Position.x = nextMove.x;
		Player->Position.y = nextMove.y;
		return 1;
	}

	if (flag == 2)
	{
		mapInfo->hardness[PointToIndex(&nextMove)] -= 85;
		return 2;
	}
	return 0;
}

int ValidateNextMove(MapInfo *mapInfo, Point nextMove, int tunnle)
{
	if (nextMove.x < 1 || nextMove.x > 79 || nextMove.y > 19 || nextMove.y < 1)
		return 0;

	unsigned char hardness = mapInfo->hardness[PointToIndex(&nextMove)];
	if (tunnle)
	{
		if (hardness == 255)
			return 0;
		else if (hardness <= 85)
			return 1; 
		else
			return 2;
	}
	else if (hardness > 0)
		return 0;
	
	return 1;

}

int PCAction(MapInfo *mapInfo, PC *Player)
{
	Point Now = Player->Position;
	Point NextMove = Now;

	int random = 0;

	if (Player->PCType == 16)
	{
		return UserAction(mapInfo, Player);
	}

	if (Player->PCType & 0b00100 || CheckOfSight(mapInfo, Player->Position))
	{
		Player->LastKnownPC = mapInfo->Player.Position;
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
				NextMove = mapInfo->TunnelPath.ShortestPath[PointToIndex(&Now)];
			}
			else
			{
				NextMove = mapInfo->nonTunnelPath.ShortestPath[PointToIndex(&Now)];
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
				NextMove.x--;
			if (Player->Position.x < mapInfo->Player.Position.x)
				NextMove.x++;


			if (Player->Position.y > mapInfo->Player.Position.y)
				NextMove.y--;
			if (Player->Position.y < mapInfo->Player.Position.y)
				NextMove.y++;
		}
		
	}

	if ((Player->PCType & 0b00001 && rand() % 2) || random)
	{
		do
		{
			NextMove = randomMove(Player->Position);

		} while (!ValidateNextMove(mapInfo, NextMove, (Player->PCType & 0b00010)));
	}

	
	return MovePC(mapInfo, Player, NextMove);

	
}

Point randomMove(Point from)
{
	Point result = from;
	switch (rand() % 8)
	{
		case 0:
			result.x++;
			break;

		case 1:
			result.x--;
			break;

		case 2:
			result.y++;
			break;

		case 3:
			result.y--;
			break;

		case 4:
			result.x++;
			result.y++;
			break;

		case 5:
			result.x++;
			result.y--;
			break;

		case 6:
			result.x--;
			result.y++;
			break;

		case 7:
			result.x--;
			result.y--;
			break;
	}

	return result;
}

int CheckOfSight(MapInfo *mapInfo, Point position)
{

	int count = 0;
	Point current;
	current.x = position.x;
	current.y = position.y;
	
	int yMin = INT_MAX;
	int yMax = INT_MAX;

	
	while (mapInfo->hardness[PointToIndex(&current)] == 0)
	{
		current.x--;

		if (mapInfo->map[PointToIndex(&current)] == '@')
			return 1;

		while (mapInfo->hardness[PointToIndex(&current)] == 0 && count <= yMin)
		{
			current.y--;
			if (mapInfo->map[PointToIndex(&current)] == '@')
				return 1;

			count++;
		}
		current.y = position.y;
		yMin = count;
		count = 0;

		while (mapInfo->hardness[PointToIndex(&current)] == 0 && count <= yMax)
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

	while (mapInfo->hardness[PointToIndex(&current)] == 0)
	{
		
		while (mapInfo->hardness[PointToIndex(&current)] == 0  && count <= yMin)
		{
			current.y--;
			if (mapInfo->map[PointToIndex(&current)] == '@')
				return 1;

			count++;
		}
		current.y = position.y;
		yMin = count;
		count = 0;

		while (mapInfo->hardness[PointToIndex(&current)] == 0 && count <= yMax)
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



void printHeap(Heap *heap)
{
	int i;
	mvprintw(26, 0, "                                                                           ");
	for (i = 0; i < heap->size; i++)
	{
		mvprintw(26, 6*i,"%d%c ", ((PCEvent*)(heap->data[i].data))->Round, ((PCEvent*)(heap->data[i].data))->Player->symbol);

		if (i % 2 == 0 && i!=4  && i != 8 && i!= 10)
			mvprintw(26, 6*(i+1) - 1, "|");


	}
}