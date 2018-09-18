#ifndef DUNGEONGENERATOR_H_   
#define DUNGEONGENERATOR_H_

#include "main.h"

MapInfo* GenerateNewMap();
void generateHallway(MapInfo *mapInfo);
void printHallway(MapInfo *mapInfo);
void placeHallway(char *map, Point start, Point end);
void placeRoom(char* map, Room *room);
int CheckEnoughRoom(char *map, Point start, Size size);
void findRoomRandomPoint(Room room, Point *point);
void cleanInvalidRoom(MapInfo *mapInfo);
void updateHardness(MapInfo *mapinfo);
void printHardness(MapInfo *mapInfo);
void placeRoomWorker(char* map, Room *room);
Room* CreateNewRooms(int nums);
MapInfo* CreateNewMapInfo();
int PointToIndex(Point* point);
Point IndexToPoint(int index);
void generatePlayerPosition(MapInfo *mapInfo);
int CoordinatesToIndex(int x, int y);

#endif 