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

MapInfo GenerateNewMap()
{
    int i;
    srand(time(NULL));
    char *map = (char*) malloc(HEIGHT * WIDTH * sizeof(char));

	MapInfo mapInfo;
	mapInfo.numsRoom = rand() % NUMS_ROOM_RANDOM + NUMS_ROOM_BASE;
    Room *rooms = (Room*) malloc(mapInfo.numsRoom * sizeof(Room));
	Point *hallwayPoint = (Point*)malloc(mapInfo.numsRoom/3+1 * sizeof(Point));
	int totalRoomArea = (rand() % TOTAL_ROOM_AREA_RANDOM + TOTAL_ROOM_AREA_BASE) * WIDTH * HEIGHT / 100;
	int AvgArea = totalRoomArea / mapInfo.numsRoom;


	mapInfo.HallwayPoints = hallwayPoint;
	mapInfo.rooms = rooms;
	mapInfo.size.dx = WIDTH;
	mapInfo.size.dy = HEIGHT;
	mapInfo.map = map;

	for (i = 0; i< HEIGHT * WIDTH; i++)
	{
		map[i] = ' ';
	}
    for(i=0; i< mapInfo.numsRoom; i++)
    {
        rooms[i].topLeft.x = 255;
		rooms[i].topLeft.y = 255;
    }

    for(i=0; i < mapInfo.numsRoom; i++)
    {
        rooms[i].size.dx = (int) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
        rooms[i].size.dy = (int) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
		rooms[i].floor = (Point *)malloc(rooms[i].size.dx * rooms[i].size.dy * sizeof(Point));
		placeRoom(map, &rooms[i]);
    }

	
	cleanInvalidRoom(&mapInfo);

	generateHallway(mapInfo);
	printHallway(map);

	return mapInfo;
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

void generateHallway(MapInfo mapInfo)
{
	int i;
	//printf("Numbers of Room: %d\n", mapInfo.numsRoom);
	Point RoomPoint[4];
	for (i = 0; i < mapInfo.numsRoom;i++)
	{
		//printf("Room %d, x: %d, y: %d\n", i, mapInfo.rooms[i].topLeft.x, mapInfo.rooms[i].topLeft.y);

		findRoomRandomPoint(mapInfo.rooms[i], &RoomPoint[0]);
		findRoomRandomPoint(mapInfo.rooms[i], &RoomPoint[1]);
		findRoomRandomPoint(mapInfo.rooms[(i + 1) % mapInfo.numsRoom], &RoomPoint[2]);
		findRoomRandomPoint(mapInfo.rooms[(i + 2) % mapInfo.numsRoom], &RoomPoint[3]);

		//printf("Hallway Point %d: (%d, %d) to (%d,%d)\n",i, RoomPoint[0].x, RoomPoint[0].y, RoomPoint[2].x, RoomPoint[2].y);
		placeHallway(mapInfo.map, RoomPoint[0], RoomPoint[2]);
		// if (i % 3 == 0)
			//placeHallway(mapInfo.map, RoomPoint[1], RoomPoint[3]);
	}
}

void findRoomRandomPoint(Room room, Point *point)
{

	point->x = rand() % room.size.dx + room.topLeft.x;
	point->y = rand() % room.size.dy + room.topLeft.y;
	
}

void printHallway(char *map)
{
	int i, j;
	printf("----------------------------------------------------------------------------------\n");
	for (i = 0; i<HEIGHT; i++)
	{
		printf("|");
		for (j = 0; j<WIDTH; j++)
		{
			printf("%c", map[i * WIDTH + j]);
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------------------\n");
}

void placeHallway(char *map, Point start, Point end)
{
	int current_x = start.x;
	int current_y = start.y;
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

			if (map[current_y*WIDTH + current_x] == ' ')
				map[current_y*WIDTH + current_x] = '#';
			else if (map[current_y*WIDTH + current_x] == '#')
				break;
		}

		random = rand() % 3 + 1;
		for (i = 0; i < random; i++)
		{
			if (current_y - end.y > 0)
				current_y--;
			else if (current_y - end.y < 0)
				current_y++;

			if (map[current_y*WIDTH + current_x] == ' ')
				map[current_y*WIDTH + current_x] = '#';
			else if (map[current_y*WIDTH + current_x] == '#')
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
        if(CheckEnoughRoom(map, start, room[0].size) == 1)
        {
			room[0].topLeft = start;

			Size min;

            for(i=0; i<room[0].size.dy; i++)
            {
                for (j = 0; j < room[0].size.dx; j++)
                {
                    map[(start.y + i)*WIDTH + start.x + j] = '.';
                }
            }

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

int CheckEnoughRoom(char *map, Point start, Size size)
{
    int i, j, current;
    for(i=0; i<size.dy; i++)
    {
        for(j=0; j<size.dx; j++)
        {
            current = (start.y + i)*WIDTH + start.x + j;
            if(start.y+i >= 20 || start.x +j >= 79 || start.y+i <= 0 || start.x+j <= 0 || map[current] != ' ' || map[current -1] != ' ' || map[current + 1] != ' ' || map[current + WIDTH] != ' ' || map[current-WIDTH] != ' ' || map[current - WIDTH + 1] != ' ' || map[current - WIDTH - 1] != ' ' || map[current + WIDTH + 1] != ' ' || map[current + WIDTH - 1] != ' ')
            {
                return 0;
            }
        }
    }
    return 1;
}
