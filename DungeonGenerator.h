#ifndef DUNGEONGENERATOR_H_   /* Include guard */
#define DUNGEONGENERATOR_H_

typedef struct
{
	int x;
	int y;
}Point;

typedef struct
{
	int dx;
	int dy;

}Size;

typedef struct
{
	Point topLeft;
	int roomType;
	Size size;
	Point *floor;
}Room;


typedef struct
{
	Room *rooms;
	int numsRoom;
	Point *HallwayPoints;
	int numsHallwayPoint;
	Size size;
	char *map; 

}MapInfo;



MapInfo GenerateNewMap();
void generateHallway(MapInfo mapInfo);
void generateHallwayPoints(MapInfo mapInfo);
void printHallway(char *map);
void placeHallway(char *map, Point start, Point end);
void placeRoom(char* map, Room *room);
int CheckEnoughRoom(char *map, Point start, Size size);
int placePlanChecker(char *map, int x, int y, Room room, Size min);

#endif 