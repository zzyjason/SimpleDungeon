#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "DungeonGenerator.h"
#include "ProfileManager.h"


int main(int argc, char **argv)
{
	int loadFlag = 0;
	int saveFlag = 0;
	int i;
	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--save"))
			saveFlag = 1;
		else if (strcmp(argv[i], "--load"))
			loadFlag = 1;
	}
	MapInfo info 
	if(loadFlag == 0)
	{
	}

	CreatProfileDir();
    MapInfo info = GenerateNewMap();
    SaveProfile(&info);

	MapInfo loaded;

    LoadProfile(&loaded);
	free(info.map);
	free(info.rooms);
    return 0;
}
