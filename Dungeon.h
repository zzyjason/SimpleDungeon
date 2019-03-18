#ifndef DUNGEON_H_
#define DUNGEON_H_

#include "Util.h"
#include "Path.h"
#include "Character.h"
#include <vector>



class Room
{
	public:
		Point TopLeft;
		Size Sizes;
		Room();
};

class Path;

class Dungeon
{

	private:

		void PlaceRoom(Room &room);
		bool CheckEnoughRoom(Point start, Size size);

		void CleanInvalidRoom();

		void GenerateHallway();
		void FindRoomRandomPoint(Room &room, Point &point);
		void PlaceHallway(Point start, Point end);
		void UpdateHardness();
		void MarkAllUnFog(int x, int y, int m, int n);

	public:
		Room *Rooms;
		unsigned char NumsRoom;

		char FogMap[80 * 21];
		char Map[80 * 21];
		Item* ItemMap[80 * 21];
		char MapLayout[80 * 21];
		unsigned char Hardness[80 * 21];
		std::vector<MonsterTemplate> mTemplate;
		std::vector<ItemTemplate> iTemplate;
		int Level;

		Path NonTunnel;
		Path Tunnel;

		PC Player;
		int NumMonster;
		NPC *Monsters;

		std::vector<Item> Items;

		Dungeon();
		void GenerateNew();
		void PrintHallway();
		void GenerateRooms();
		void PlaceRoomWorker(Room &room);
		void GenerateStairs();
		void GenerateStairsWorkers(char symbol, int count);
		void GenerateMonster();
		void PlaceAllPlayer();
		int GeneratePlayerPosition(Character &character, int exclude);
		void PrintFogHallway();
		void UpdateFogHallway();
		bool CheckValidPoint(int x, int y);
		void GenerateItem();
		void PlaceItems();
		void PlaceItemsWorker(Room &room, Item &item);
};

#endif