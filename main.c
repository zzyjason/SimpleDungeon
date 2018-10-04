#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "Path.h"

#include "DungeonGenerator.h"
#include "ProfileManager.h"
#include "Heap.h"
#include "TurnManger.h"


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

	mapInfo->numMonster = numMon;
	GenerateMonster(mapInfo);
	placeAllPlayerPosition(mapInfo);

	UpdatePath(mapInfo);

	printHallway(mapInfo);
	printHardness(mapInfo);
	printPath(mapInfo);

	Heap* turn = CreateTurnManager(mapInfo);
	
	while (NextTurn(mapInfo, turn) != 0)
	{
		UpdatePath(mapInfo);
	}

	printf("Dead\n");


	free(mapInfo->map);
	free(mapInfo->rooms);
	free(mapInfo->Monsters);
	free(mapInfo);
    return 0;
}


