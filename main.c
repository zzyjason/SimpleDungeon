#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "Path.h"

#include "DungeonGenerator.h"
#include "ProfileManager.h"
#include "Heap.h"
#include "TurnManager.h"
#include <ncurses.h>


int main(int argc, char **argv)
{
	srand(time(NULL));

	int loadFlag = 0;
	int saveFlag = 0;
	int i,j;
	int numMon = rand() % 5 + 8;

	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--save") == 0)
			saveFlag = 1;
		else if (strcmp(argv[i], "--load") == 0)
			loadFlag = 1;
		else if (strcmp(argv[i], "--nummon") == 0 && i + 1 < argc)
		{
			numMon = 0;
			for (j = 0; j < strlen(argv[i + 1]); j++)
			{
				if (argv[i + 1][j] > 47 && argv[i + 1][j] < 58)
					numMon = numMon * 10 + argv[i + 1][j] - 48;
				else
				{
					numMon = rand() % 5 + 8;
					break;
				}
			}
		}
	}

	MapInfo *mapInfo = NULL;


	if (loadFlag)
	{
		mapInfo = CreateNewMapInfo();
		LoadProfile(mapInfo);
	}
	else
		mapInfo = GenerateNewMap();

	if (saveFlag)
	    SaveProfile(mapInfo);

	generateStairs(mapInfo);

	mapInfo->numMonster = numMon;
	GenerateMonster(mapInfo);
	placeAllPlayerPosition(mapInfo);


	UpdatePath(mapInfo);

	//printHallway(mapInfo);
	//printHardness(mapInfo);
	//printPath(mapInfo);

	Heap* turn = CreateTurnManager(mapInfo);
	
	WINDOW* scr = initscr();
	curs_set(0);
	int result;

	do
	{
		result = NextTurn(mapInfo, turn);
		if (result != 0)
			continue;

		while (turn->size != 0)
			free(pop(turn));
		free(mapInfo->map);
		free(mapInfo->mapLayout);
		free(mapInfo->rooms);
		free(mapInfo->Monsters);
		free(mapInfo);
		mapInfo = GenerateNewMap();
		generateStairs(mapInfo);
		mapInfo->numMonster = numMon;
		GenerateMonster(mapInfo);
		placeAllPlayerPosition(mapInfo);
		free(turn->data);
		free(turn);
		turn = CreateTurnManager(mapInfo);

	} while (result >= 0);

	switch (result)
	{
		case -1:
			mvprintw(23, 0, "Dead");
			break;

		case -2:
			mvprintw(23, 0, "Quit");
			break;
	}

	refresh();
	getch();
	
	endwin();
	delwin(scr);


	
	while (turn->size != 0)
		free(pop(turn));
	free(turn->data);
	free(turn);
	free(mapInfo->map);
	free(mapInfo->mapLayout);
	free(mapInfo->rooms);
	free(mapInfo->Monsters);
	free(mapInfo);
    return 0;
}


