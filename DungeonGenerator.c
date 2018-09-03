#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "DungeonGenerator.h"

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
	mapInfo.numsHallwayPoint = mapInfo.numsRoom / 3 + 1;
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
        rooms[i].topLeft.x = -1;
		rooms[i].topLeft.y = -1;
    }

    for(i=0; i < mapInfo.numsRoom; i++)
    {
        rooms[i].roomType = rand() % 11;
        rooms[i].size.dx = (int) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
        rooms[i].size.dy = (int) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
		rooms[i].floor = (Point *)malloc(rooms[i].size.dx * rooms[i].size.dy * sizeof(Point));
		placeRoom(map, &rooms[i]);
    }

	generateHallwayPoints(mapInfo);
	generateHallway(mapInfo);
	printHallway(map);

	return mapInfo;
}

void generateHallway(MapInfo mapInfo)
{
	int i, x, y, dx, dy, startX, startY, nextHallwayPoint;
	for (i = 0; i<mapInfo.numsRoom; i++)
	{
		int randomSkip = rand() % 100;

		x = mapInfo.rooms[i].topLeft.x;
		y = mapInfo.rooms[i].topLeft.y;
		startX = x;
		startY = y;
		dx = mapInfo.rooms[i].size.dx;
		dy = mapInfo.rooms[i].size.dy;

		if (startX == -1 || startY == -1)
		{
			continue;
		}

		while (mapInfo.map[y * WIDTH + x] == ' ')
		{
			x++;
			if (x>startX + dx)
			{
				x = startX;
				y++;
			}
		}

		int dir = 0;
		while (randomSkip != 0)
		{
			switch (dir)
			{
			case 0:
				if (y - 1 >= startY && mapInfo.map[(y - 1) * WIDTH + x] == '.')
				{
					dir = 3;
					break;
				}
				else if (x + 1>startX + dx || mapInfo.map[y * WIDTH + x + 1] != '.')
				{
					dir = 1;
					break;
				}
				x++;
				break;
			case 1:
				if (x + 1 <= startX + dx && mapInfo.map[y * WIDTH + x + 1] == '.')
				{
					dir = 0;
					break;
				}
				else if (y + 1>startY + dy || mapInfo.map[(y + 1) * WIDTH + x] != '.')
				{
					dir = 2;
					break;
				}
				y++;
				break;
			case 2:
				if (y + 1 <= startY + dy && mapInfo.map[(y + 1) * WIDTH + x] == '.')
				{
					dir = 1;
					break;
				}
				else if (x - 1<startX || mapInfo.map[y * WIDTH + x - 1] != '.')
				{
					dir = 3;
					break;
				}
				x--;
				break;
			case 3:
				if (x - 1 >= startX && mapInfo.map[y * WIDTH + x - 1] == '.')
				{
					dir = 2;
					break;
				}
				else if (y - 1<startY || mapInfo.map[(y - 1) * WIDTH + x] != '.')
				{
					dir = 0;
					break;
				}
				y--;
				break;
			}
			randomSkip--;
		}

		nextHallwayPoint = i % (mapInfo.numsHallwayPoint);
		Point currentRandomPoint;
		currentRandomPoint.x = x;
		currentRandomPoint.y = y;
		placeHallway(mapInfo.map,currentRandomPoint, mapInfo.HallwayPoints[nextHallwayPoint]);

		if (i % 3 == 0)
		{
			nextHallwayPoint = (i + 1) % (mapInfo.numsHallwayPoint);
			placeHallway(mapInfo.map, currentRandomPoint, mapInfo.HallwayPoints[nextHallwayPoint]);
		}

	}
}

void generateHallwayPoints(MapInfo mapInfo)
{
	int i, x, y;
	for (i = 0; i<mapInfo.numsHallwayPoint; i++)
	{
		do {
			x = rand() % 78 + 1;
			y = rand() % 22 + 1;
		} while (mapInfo.map[y * WIDTH + x] != ' ');
		mapInfo.HallwayPoints[i].x = x;
		mapInfo.HallwayPoints[i].y = y;
		mapInfo.map[y * WIDTH + x] = '#';
	}
}

void printHallway(char *map)
{
	int i, j;
	printf("----------------------------------------------------------------------------------\n");
	for (i = 0; i<21; i++)
	{
		printf("|");
		for (j = 0; j<80; j++)
		{
			printf("%c", map[i * 80 + j]);
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------------------\n");
}

void placeHallway(char *map, Point start, Point end)
{
    int current_x = start.x;
    int current_y = start.y;
    while(current_x != end.x || current_y != end.y)
    {
        if(current_x - end.x > 0)
            current_x--;
        else if(current_x - end.x < 0)
            current_x++;

        if(map[current_y*80 + current_x] == ' ')
            map[current_y*80 + current_x] = '#';

        if(current_y - end.y > 0)
            current_y--;
        else if(current_y - end.y < 0)
            current_y++;

        if(map[current_y*80 + current_x] == ' ')
            map[current_y*80 + current_x] = '#';
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

			min.dx = rand() % (room[0].size.dx / 2);
			min.dy = rand() % (room[0].size.dy / 2);

            if(min.dx<3)
				min.dx = 3;

            if(min.dy<3)
				min.dy = 3;


            for(i=0; i<room[0].size.dy; i++)
            {
                for (j = 0; j < room[0].size.dx; j++)
                {
                    if (placePlanChecker(map, j, i, room[0], min))
                    {
                        map[(start.y + i)*WIDTH + start.x + j] = '.';
                    }
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
            if(start.y+i >= 20 || start.x +j >= 79 || start.y+i <= 0 || start.x+j <= 0 || map[current] != ' ' || map[current -1] != ' ' || map[current + 1] != ' ' || map[current + WIDTH] != ' ' || map[current-WIDTH] != ' ' )
            {
                return 0;
            }
        }
    }
    return 1;
}

int placePlanChecker(char *map, int x, int y, Room room, Size min)
{
    if(room.roomType == 10)
    {
        return 1;
    }
	int minX = min.dx;
	int minY = min.dy;
    int extraSpaceX = (room.size.dx-minX) / 2;
    int extraSpaceY = (room.size.dy-minY) / 2;

    switch(room.roomType)
    {
        case 0:
            if((y>=extraSpaceY && y<=extraSpaceY + minY) || (x>=extraSpaceX && x<=extraSpaceX + minX))
            {
                return 1;
            }
            break;
        case 1:
            if(y<=minY || (x>=extraSpaceX && x<=extraSpaceX + minX))
            {
                return 1;
            }
            break;
        case 2:
            if((y>=extraSpaceY && y<=extraSpaceY + minY) || (x>=extraSpaceX*2))
            {
                return 1;
            }
            break;
        case 3:
            if((y>=extraSpaceY && y<=extraSpaceY + minY) || (x<=minX))
            {
                return 1;
            }
            break;
        case 4:
            if(y>=extraSpaceY*2 || (x>=extraSpaceX && x<=minX + extraSpaceX))
            {
                return 1;
            }
            break;
        case 5:
            if(y<=minY || x<=minX)
            {
                return 1;
            }
            break;
        case 6:
            if(y<=minY || x>=extraSpaceX*2)
            {
                return 1;
            }
            break;
        case 7:
            if(y>=extraSpaceY*2 || x<=minX)
            {
                return 1;
            }
            break;
        case 8:
            if(y>=extraSpaceY*2 || x>=extraSpaceX*2)
            {
                return 1;
            }
            break;
        case 9:
            if(y<=minY/2 || x<=minX/2 || x>= room.size.dx-minX/2)
            {
                return 1;
            }
    }
    return 0;
}



