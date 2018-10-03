#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


#include "DungeonGenerator.h"
#include "ProfileManager.h"

char *profileDir = NULL;
char *profile = NULL;

void LoadProfile(MapInfo *mapInfo)
{
	if (profileDir == NULL)
		CreatProfileDir();

	FILE *file = fopen(profile, "r");
	if (file == NULL)
	{
		return;
	}

	char *Title = (char*)malloc(12);
	fread(Title, 1, 12, file);

	int fileVersion = 0;
	fread(&fileVersion, sizeof(int), 1, file);
	fileVersion = be32toh(fileVersion);

	int fileSize = 0;
	fread(&fileSize, sizeof(int), 1, file);
	fileSize = be32toh(fileSize);

	fread(&mapInfo->Player.Position.x, 1, 1, file);
	fread(&mapInfo->Player.Position.y, 1, 1, file);
	fread(&mapInfo->hardness, 1, 80 * 21, file);

	int BytesToRead = fileSize - ftell(file);
	int i = 0;
	mapInfo->numsRoom = BytesToRead / 4;

	mapInfo->rooms = CreateNewRooms(mapInfo->numsRoom);

	for (i=0; i < mapInfo->numsRoom; i++)
	{
		fread(&mapInfo->rooms[i].topLeft.x, 1, 1, file);
		fread(&mapInfo->rooms[i].topLeft.y, 1, 1, file);
		fread(&mapInfo->rooms[i].size.dx, 1, 1, file);
		fread(&mapInfo->rooms[i].size.dy, 1, 1, file);
		placeRoomWorker(mapInfo->map, &mapInfo->rooms[i]);
	}

	fclose(file);
	loadHallway(mapInfo);
}

void loadHallway(MapInfo *mapInfo)
{
	int i;
	for (i = 0; i < 80 * 21; i++)
	{
		if (mapInfo->map[i] == ' ' && mapInfo->hardness[i] == 0)
			mapInfo->map[i] = '#';
	}
}

void SaveProfile(MapInfo *mapInfo)
{
	if(profileDir == NULL)
		CreatProfileDir();

	FILE *file = fopen(profile, "w");
	int fileVersion = htobe32(555);
	
	int fileSize = 1702 + ((int)mapInfo->numsRoom) * 4;
	fileSize = htobe32(fileSize);


	fprintf(file, "RLG327-F2018");
	fwrite(&fileVersion, sizeof(int), 1, file);
	fwrite(&fileSize, sizeof(int), 1, file);
	fwrite(&mapInfo->Player.Position.x, 1, 1, file);
	fwrite(&mapInfo->Player.Position.y, 1, 1, file);

	fwrite(&mapInfo->hardness, 1, 80 * 21, file);

	int i = 0;
	for (; i < mapInfo->numsRoom; i++)
	{
		fwrite(&mapInfo->rooms[i].topLeft.x, 1, 1, file);
		fwrite(&mapInfo->rooms[i].topLeft.y, 1, 1, file);
		fwrite(&mapInfo->rooms[i].size.dx, 1, 1, file);
		fwrite(&mapInfo->rooms[i].size.dy, 1, 1, file);
	}
	
	fclose(file);
}


void CreatProfileDir()
{
	GetProfileDir();
	if (stat(profileDir, NULL) == -1)
		mkdir(profileDir, 0770);
}

void GetProfileDir()
{
	char *homedir = getenv("HOME");
	size_t homedirlen = strlen(homedir);
	char *result = (char*)malloc(sizeof(char) * (homedirlen + 10));
	profile = (char*)malloc(sizeof(char) * (homedirlen + 20));

	strcpy(result, homedir);
	strcat(result, "/.rlg327/");

	strcpy(profile, result);
	strcat(profile, "dungeon");
	profileDir = result;

}