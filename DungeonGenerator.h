#ifndef DUNGEONGENERATOR_H_   
#define DUNGEONGENERATOR_H_

typedef struct
{
	unsigned char x;
	unsigned char y;
}Point;

typedef struct
{
	Point Position;
	unsigned char Hardness;
}Block;

typedef struct
{
	unsigned char dx;
	unsigned char dy;

}Size;

typedef struct
{
	Point topLeft;
	Size size;
	Point *floor;
}Room;


typedef struct
{
	Room *rooms;
	unsigned char numsRoom;
	Point *HallwayPoints;
	Size size;
	char *map; 
	unsigned char Hardness[80*21];

}MapInfo;

MapInfo GenerateNewMap();
void generateHallway(MapInfo mapInfo);
void printHallway(char *map);
void placeHallway(char *map, Point start, Point end);
void placeRoom(char* map, Room *room);
int CheckEnoughRoom(char *map, Point start, Size size);
void findRoomRandomPoint(Room room, Point *point);
void cleanInvalidRoom(MapInfo *mapInfo);

#endif 