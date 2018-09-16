#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#include "DungeonGenerator.h"
#include "ProfileManager.h"


int main(int argc, char **argv)
{
	srand(time(NULL));

	int loadFlag = 0;
	int saveFlag = 0;
	int i;

	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--save") == 0)
			saveFlag = 1;
		else if (strcmp(argv[i], "--load") == 0)
			loadFlag = 1;
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

	printHallway(mapInfo->map);


	free(mapInfo->map);
	free(mapInfo->rooms);
    return 0;
}


