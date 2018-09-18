#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "DungeonGenerator.h"

#include <string.h>

#define HEIGHT 21
#define WIDTH 80

#define NUMS_ROOM_BASE 7
#define NUMS_ROOM_RANDOM 5
#define TOTAL_ROOM_AREA_BASE 20
#define TOTAL_ROOM_AREA_RANDOM 10

MapInfo* CreateNewMapInfo()
{
	int i;
	MapInfo *newMapInfo = (MapInfo*)malloc(sizeof(MapInfo));
	newMapInfo->map = (char*)malloc(HEIGHT * WIDTH * sizeof(char));
	for(i=0; i< HEIGHT * WIDTH; i++)
	{
		newMapInfo->map[i] = ' ';
		newMapInfo->hardness[i] = 0;
		newMapInfo->TunnelPath.Distance[i] = 65535;
		newMapInfo->nonTunnelPath.Distance[i] = 65535;
		
	}
	newMapInfo->numsRoom = 0;
	newMapInfo->playerPosition.x = 1;
	newMapInfo->playerPosition.y = 1;
	newMapInfo->size.dx = WIDTH;
	newMapInfo->size.dy = HEIGHT;
	newMapInfo->rooms = NULL;
	


	return newMapInfo;
}

Room* CreateNewRooms(int nums)
{
	Room *rooms = (Room*)malloc(nums * sizeof(Room));
	int i;
	for(i=0; i<nums; i++)
	{
		rooms[i].topLeft.x = 255;
		rooms[i].topLeft.y = 255;
		rooms[i].size.dx = 0;
		rooms[i].size.dy = 0;
	}

	return rooms;
}


MapInfo* GenerateNewMap()
{
	int i;
	MapInfo *mapInfo = CreateNewMapInfo();

	mapInfo->numsRoom = rand() % NUMS_ROOM_RANDOM + NUMS_ROOM_BASE;
    mapInfo->rooms = CreateNewRooms(mapInfo->numsRoom);

	mapInfo->size.dx = WIDTH;
	mapInfo->size.dy = HEIGHT;
	mapInfo->playerPosition.x = 1;
	mapInfo->playerPosition.y = 1;

	int totalRoomArea = (rand() % TOTAL_ROOM_AREA_RANDOM + TOTAL_ROOM_AREA_BASE) * WIDTH * HEIGHT / 100;
	int AvgArea = totalRoomArea / mapInfo->numsRoom;
	
    for(i=0; i < mapInfo->numsRoom; i++)
    {
        mapInfo->rooms[i].size.dx = (unsigned char) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
        mapInfo->rooms[i].size.dy = (unsigned char) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
		placeRoom(mapInfo->map, &mapInfo->rooms[i]);
    }

	cleanInvalidRoom(mapInfo);
	generateHallway(mapInfo);
	generatePlayerPosition(mapInfo);
	updateHardness(mapInfo);

	return mapInfo;
}

void printHardness(MapInfo *mapInfo)
{
	int i, j;
	printf("----------------------------------------------------------------------------------\n");
	for (i = 0; i<HEIGHT; i++)
	{
		printf("|");
		for (j = 0; j<WIDTH; j++)
		{
			int print = 0;
			if (mapInfo->hardness[CoordinatesToIndex(j, i)] == 255)
				print = 9;
			else if (mapInfo->hardness[CoordinatesToIndex(j, i)] > 0)
				print = mapInfo->hardness[CoordinatesToIndex(j, i)]/85 + 1;
			printf("%d", print);
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------------------\n");
}

void updateHardness(MapInfo *mapInfo)
{
	int i = 0;
	for (; i < 80 * 21; i++)
	{
		switch (mapInfo->map[i])
		{
			case ' ':
				mapInfo->hardness[i] = rand()%253 + 1;
				break;
			case '#':
				mapInfo->hardness[i] = 0;
				break;
			case '.':
				mapInfo->hardness[i] = 0;
			case '@':
				mapInfo->hardness[i] = 0;
			default:
				mapInfo->hardness[i] = 0;
		}

		if ((i + 1) % 80 < 2 || i <80 || i>= 80*20)
			mapInfo->hardness[i] = 255;
	}
}

void cleanInvalidRoom(MapInfo *mapInfo)
{
	Room *result = (Room*)malloc(mapInfo->numsRoom * sizeof(Room));
	Room *current = result;
	int i,count=0;
	for (i = 0; i < mapInfo->numsRoom; i++)
	{
		if (mapInfo->rooms[i].topLeft.x != 255 && mapInfo->rooms[i].topLeft.y != 255)
		{
			memcpy(current, &mapInfo->rooms[i], sizeof(Room));
			current++;
			count++;
		}
	}
	memcpy(mapInfo->rooms, result, sizeof(Room) * count);
	mapInfo->numsRoom = count;
}

void generatePlayerPosition(MapInfo *mapInfo)
{
	int RandomRoomPlayerIn = rand() % mapInfo->numsRoom - 1;
	findRoomRandomPoint(mapInfo->rooms[RandomRoomPlayerIn], &mapInfo->playerPosition);
	mapInfo->map[PointToIndex(&mapInfo->playerPosition)] = '@';
}

void generateHallway(MapInfo *mapInfo)
{
	int i;
	//printf("Numbers of Room: %d\n", mapInfo.numsRoom);
	Point RoomPoint[4];

	for (i = 0; i < mapInfo->numsRoom;i++)
	{
		//printf("Room %d, x: %d, y: %d\n", i, mapInfo.rooms[i].topLeft.x, mapInfo.rooms[i].topLeft.y);

		findRoomRandomPoint(mapInfo->rooms[i], &RoomPoint[0]);
		findRoomRandomPoint(mapInfo->rooms[i], &RoomPoint[1]);
		findRoomRandomPoint(mapInfo->rooms[(i + 1) % mapInfo->numsRoom], &RoomPoint[2]);
		findRoomRandomPoint(mapInfo->rooms[(i + 2) % mapInfo->numsRoom], &RoomPoint[3]);

		//printf("Hallway Point %d: (%d, %d) to (%d,%d)\n",i, RoomPoint[0].x, RoomPoint[0].y, RoomPoint[2].x, RoomPoint[2].y);
		placeHallway(mapInfo->map, RoomPoint[0], RoomPoint[2]);
		// if (i % 3 == 0)
			//placeHallway(mapInfo.map, RoomPoint[1], RoomPoint[3]);
	}
}

void findRoomRandomPoint(Room room, Point *point)
{
	point->x = rand() % room.size.dx + room.topLeft.x;
	point->y = rand() % room.size.dy + room.topLeft.y;
}

void printHallway(MapInfo *mapInfo)
{
	int i, j;
	printf("----------------------------------------------------------------------------------\n");
	for (i = 0; i<HEIGHT; i++)
	{
		printf("|");
		for (j = 0; j<WIDTH; j++)
		{
			printf("%c", mapInfo->map[CoordinatesToIndex(j, i)]);
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------------------\n");
}

int PointToIndex(Point* point)
{

	return WIDTH * point->y + point->x;

}

int CoordinatesToIndex(int x, int y)
{
	return WIDTH * y + x;
}

Point IndexToPoint(int index)
{
	Point newPoint;
	newPoint.x = index % WIDTH;
	newPoint.y = index / WIDTH;
	return newPoint;
}

void placeHallway(char *map, Point start, Point end)
{
	unsigned char current_x = start.x;
	unsigned char current_y = start.y;
	int random = 0;
	int i;

	while (current_x != end.x || current_y != end.y)
	{
		random = rand() % 3 + 1;
		for (i = 0; i < random; i++)
		{
			if (current_x - end.x > 0)
				current_x--;
			else if (current_x - end.x < 0)
				current_x++;

			if (map[CoordinatesToIndex(current_x, current_y)] == ' ')
				map[CoordinatesToIndex(current_x, current_y)] = '#';
			else if (map[CoordinatesToIndex(current_x, current_y)] == '#')
				break;
		}

		random = rand() % 3 + 1;
		for (i = 0; i < random; i++)
		{
			if (current_y - end.y > 0)
				current_y--;
			else if (current_y - end.y < 0)
				current_y++;

			if (map[CoordinatesToIndex(current_x, current_y)] == ' ')
				map[CoordinatesToIndex(current_x, current_y)] = '#';
			else if (map[CoordinatesToIndex(current_x, current_y)] == '#')
				break;
		}
	}
}

void placeRoom(char* map, Room *room)
{
	Point start;
	start.x = rand()%74 + 1;
	start.y = rand()%18 +1;
    int counter = 0;
    int i,j;

    while(1)
    {
        counter ++;
        if(CheckEnoughRoom(map, start, room->size) == 1)
        {
			room->topLeft = start;
			
			placeRoomWorker(map, room);

            break;
        }
        else
        {
			start.x = rand()%75 + 1;
			start.y = rand()%17 +1;
        }

        if(counter > 100)
        {
            return;
        }
    }
}

void placeRoomWorker(char* map, Room *room)
{
	int i, j;

	for (i = 0; i<room->size.dy; i++)
	{
		for (j = 0; j < room->size.dx; j++)
		{
			map[CoordinatesToIndex(room->topLeft.x + j, room->topLeft.y + i)] = '.';
		}
	}
}

int CheckEnoughRoom(char *map, Point start, Size size)
{
    int i, j, current;
    for(i=0; i<size.dy; i++)
    {
        for(j=0; j<size.dx; j++)
        {
			
            current = CoordinatesToIndex(start.x + j, (start.y + i));
            if(start.y+i >= 20 || start.x+j >= 79 || start.y+i <= 0 || start.x+j <= 0 || map[current] != ' ' || map[current -1] != ' ' || map[current + 1] != ' ' || map[current + WIDTH] != ' ' || map[current-WIDTH] != ' ' || map[current - WIDTH + 1] != ' ' || map[current - WIDTH - 1] != ' ' || map[current + WIDTH + 1] != ' ' || map[current + WIDTH - 1] != ' ')
            {
                return 0;
            }
        }
    }
    return 1;
}
