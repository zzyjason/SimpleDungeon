#include "Util.h"
#include "Dungeon.h"
#include "Path.h"
#include "Character.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ncurses.h>
#include "Profile.h"


const int NUMS_ROOM_BASE = 7;
const int NUMS_ROOM_RANDOM = 5;
const int TOTAL_ROOM_AREA_BASE = 20;
const int TOTAL_ROOM_AREA_RANDOM = 10;


Room::Room()
{
	Sizes.dx = 0;
	Sizes.dy = 0;
}

Dungeon::Dungeon()
	:Player()
{
	int i;

	for (i = 0; i< HEIGHT * WIDTH; i++)
	{
		Map[i] = ' ';
		ItemMap[i] = NULL;
		FogMap[i] = ' ';
		Hardness[i] = 0;
	}

	NumsRoom = 0;
	Rooms = NULL;

}

void Dungeon::GenerateNew()
{
	int i;
	NumsRoom = rand() % NUMS_ROOM_RANDOM + NUMS_ROOM_BASE;
	GenerateRooms();

	int totalRoomArea = (rand() % TOTAL_ROOM_AREA_RANDOM + TOTAL_ROOM_AREA_BASE) * WIDTH * HEIGHT / 100;
	int AvgArea = totalRoomArea / NumsRoom;

	for (i = 0; i < NumsRoom; i++)
	{
		Rooms[i].Sizes.dx = (unsigned char)sqrt((double)((rand() % 13) * AvgArea / 10)) + 3;
		Rooms[i].Sizes.dy = (unsigned char)sqrt((double)((rand() % 13) * AvgArea / 10)) + 3;
		PlaceRoom(Rooms[i]);
	}

	CleanInvalidRoom();
	GenerateHallway();

	UpdateHardness();

	strncpy(MapLayout, Map, HEIGHT * WIDTH);

}

void Dungeon::GenerateRooms()
{
	int i;

	Rooms = (Room*)malloc(NumsRoom * sizeof(Room));
	
	for (i = 0; i<NumsRoom; i++)
	{
		Rooms[i] = Room();
	}
}

void Dungeon::PlaceRoom(Room& room)
{
	Point start;
	start.x = rand() % 74 + 1;
	start.y = rand() % 18 + 1;
	int counter = 0;


	while (1)
	{
		counter++;
		if (CheckEnoughRoom(start, room.Sizes))
		{
			room.TopLeft = start;

			PlaceRoomWorker(room);

			break;
		}
		else
		{
			start.x = rand() % 75 + 1;
			start.y = rand() % 17 + 1;
		}

		if (counter > 100)
		{
			return;
		}
	}
}

bool Dungeon::CheckEnoughRoom(Point start, Size size)
{
	int i, j, current;
	for (i = 0; i<size.dy; i++)
	{
		for (j = 0; j<size.dx; j++)
		{

			current = Point::CoordinatesToIndex(start.x + j, (start.y + i));
			if (start.y + i >= 20 || start.x + j >= 79 || start.y + i <= 0 || start.x + j <= 0 || Map[current] != ' ' || Map[current - 1] != ' ' || Map[current + 1] != ' ' || Map[current + WIDTH] != ' ' || Map[current - WIDTH] != ' ' || Map[current - WIDTH + 1] != ' ' || Map[current - WIDTH - 1] != ' ' || Map[current + WIDTH + 1] != ' ' || Map[current + WIDTH - 1] != ' ')
			{
				return false;
			}
		}
	}
	return true;
}

void Dungeon::PlaceRoomWorker(Room &room)
{
	int i, j;

	for (i = 0; i<room.Sizes.dy; i++)
	{
		for (j = 0; j < room.Sizes.dx; j++)
		{
			Map[Point::CoordinatesToIndex(room.TopLeft.x + j, room.TopLeft.y + i)] = '.';
		}
	}

}

void Dungeon::CleanInvalidRoom()
{
	Room *result = (Room*)malloc(NumsRoom * sizeof(Room));
	Room *current = result;
	int i, count = 0;
	for (i = 0; i < NumsRoom; i++)
	{
		if (Rooms[i].TopLeft.x != 255 && Rooms[i].TopLeft.y != 255)
		{
			memcpy(current, &Rooms[i], sizeof(Room));
			current++;
			count++;
		}
	}
	memcpy(Rooms, result, sizeof(Room) * count);
	NumsRoom = count;
	free(result);
}

void Dungeon::GenerateHallway()
{
	int i;

	Point RoomPoint[2];

	for (i = 0; i < NumsRoom; i++)
	{
		FindRoomRandomPoint(Rooms[i], RoomPoint[0]);
		FindRoomRandomPoint(Rooms[(i + 1) % NumsRoom], RoomPoint[1]);

		PlaceHallway(RoomPoint[0], RoomPoint[1]);
	}
}

void Dungeon::FindRoomRandomPoint(Room &room, Point &point)
{
	if (room.Sizes.dx == 0 || room.Sizes.dy == 0)
	{
		point.x = -1;
		point.y = -1;
		return;
	}

	point.x = rand() % room.Sizes.dx + room.TopLeft.x;
	point.y = rand() % room.Sizes.dy + room.TopLeft.y;
}

void Dungeon::PlaceHallway(Point start, Point end)
{
	Point current(start.x, start.y);
	int random = 0;
	int i;

	while (current.x != end.x || current.y != end.y)
	{
		random = rand() % 3 + 1;
		for (i = 0; i < random; i++)
		{
			if (current.x - end.x > 0)
				current.x--;
			else if (current.x - end.x < 0)
				current.x++;

			if (Map[current.ToIndex()] == ' ')
				Map[current.ToIndex()] = '#';
			else if (Map[current.ToIndex()] == '#')
				break;
		}

		random = rand() % 3 + 1;
		for (i = 0; i < random; i++)
		{
			if (current.y - end.y > 0)
				current.y--;
			else if (current.y - end.y < 0)
				current.y++;

			if (Map[current.ToIndex()] == ' ')
				Map[current.ToIndex()] = '#';
			else if (Map[current.ToIndex()] == '#')
				break;
		}
	}
}

void Dungeon::UpdateHardness()
{
	int i;
	for (i = 0; i < HEIGHT * WIDTH; i++)
	{
		switch (Map[i])
		{
		case ' ':
			Hardness[i] = rand() % 253 + 1;
			break;
		case '#':
			Hardness[i] = 0;
			break;
		case '.':
			Hardness[i] = 0;
		case '@':
			Hardness[i] = 0;
		default:
			Hardness[i] = 0;
		}

		if ((i + 1) % 80 < 2 || i <80 || i >= 80 * 20)
			Hardness[i] = 255;
	}
}

void Dungeon::PrintHallway()
{
	int i, j;

	for (i = 0; i<HEIGHT; i++)
	{
		j = 0;

		for (; j<WIDTH; j++)
		{
			int Monsterfound = -1;
			for (int z = 0; z < NumMonster; z++)
			{
				if (Monsters[z].Symbol == Map[Point::CoordinatesToIndex(j, i)])
					Monsterfound = z;
			}

			if (Monsterfound != -1)
			{
				attron(COLOR_PAIR(Monsters[Monsterfound].Color));
			}
			
			if(Player.Position.x == j && Player.Position.y == i)
				mvaddch(i, j, '@');
			else if(ItemMap[Point::CoordinatesToIndex(j, i)] == NULL || Monsterfound != -1)
				mvaddch(i, j, Map[Point::CoordinatesToIndex(j, i)]);
			else
			{
				attron(COLOR_PAIR(ItemMap[Point::CoordinatesToIndex(j, i)]->Color));
				mvaddch(i, j, ItemMap[Point::CoordinatesToIndex(j, i)]->Symbol);
				attroff(COLOR_PAIR(ItemMap[Point::CoordinatesToIndex(j, i)]->Color));
			}

			if (Monsterfound != -1)
			{
				attroff(COLOR_PAIR(Monsters[Monsterfound].Color));
			}

		}

	}
	mvprintw(0, 0, "----------------------------------------------------------------------------------");
}

void Dungeon::PrintFogHallway()
{
	int i, j;

	for (i = 0; i<HEIGHT; i++)
	{
		j = 0;

		for (; j<WIDTH; j++)
		{
			int Monsterfound = -1;
			for (int z = 0; z < NumMonster; z++)
			{
				if (Monsters[z].Symbol == Map[Point::CoordinatesToIndex(j, i)])
					Monsterfound = z;
			}

			if (Monsterfound != -1)
			{
				attron(COLOR_PAIR(Monsters[Monsterfound].Color));
			}

			if (Player.Position.x == j && Player.Position.y == i)
				mvaddch(i, j, '@');
			else if (ItemMap[Point::CoordinatesToIndex(j, i)] == NULL || Monsterfound != -1 || FogMap[Point::CoordinatesToIndex(j, i)] == ' ')
				mvaddch(i, j, FogMap[Point::CoordinatesToIndex(j, i)]);
			else
			{
				attron(COLOR_PAIR(ItemMap[Point::CoordinatesToIndex(j, i)]->Color));
				mvaddch(i, j, ItemMap[Point::CoordinatesToIndex(j, i)]->Symbol);
				attroff(COLOR_PAIR(ItemMap[Point::CoordinatesToIndex(j, i)]->Color));
			}

			if (Monsterfound != -1)
			{
				attroff(COLOR_PAIR(Monsters[Monsterfound].Color));
			}

		}

	}
	mvprintw(0, 0, "----------------------------------------------------------------------------------");
}

void Dungeon::UpdateFogHallway()
{
	int i, target;
	bool q = true, w = true, e = true, a = true, d = true, z = true, x = true, c = true;
	for (i = 0; i < HEIGHT * WIDTH; i++)
	{
		if (FogMap[i] == '@')
			FogMap[i] = MapLayout[i];
	}


	FogMap[Player.Position.ToIndex()] = '@';
	for (i = 1; i <= 2; i++)
	{
		if (q && CheckValidPoint(Player.Position.x - 1 * i, Player.Position.y - 1 * i))
		{
			target = Point::CoordinatesToIndex(Player.Position.x - 1 * i, Player.Position.y - 1 * i);
			FogMap[target] = Map[target];
		}
		else
		{
			q = false;
		}


		if (w && CheckValidPoint(Player.Position.x, Player.Position.y - 1 * i))
		{
			target = Point::CoordinatesToIndex(Player.Position.x, Player.Position.y - 1 * i);
			FogMap[target] = Map[target];
		}
		else
		{
			w = false;
		}

		if (e && CheckValidPoint(Player.Position.x + 1 * i, Player.Position.y - 1 * i))
		{
			target = Point::CoordinatesToIndex(Player.Position.x + 1 * i, Player.Position.y - 1 * i);
			FogMap[target] = Map[target];
		}
		else
		{
			e = false;
		}

		if (a && CheckValidPoint(Player.Position.x - 1 * i, Player.Position.y))
		{
			target = Point::CoordinatesToIndex(Player.Position.x - 1 * i, Player.Position.y);
			FogMap[target] = Map[target];
		}
		else
		{
			a = false;
		}

		if (d && CheckValidPoint(Player.Position.x + 1 * i, Player.Position.y))
		{
			target = Point::CoordinatesToIndex(Player.Position.x + 1 * i, Player.Position.y);
			FogMap[target] = Map[target];
		}
		else
		{
			d = false;
		}

		if (z && CheckValidPoint(Player.Position.x - 1 * i, Player.Position.y + 1 * i))
		{
			target = Point::CoordinatesToIndex(Player.Position.x - 1 * i, Player.Position.y + 1 * i);
			FogMap[target] = Map[target];
		}
		else
		{
			z = false;
		}

		if (x && CheckValidPoint(Player.Position.x, Player.Position.y + 1 * i))
		{
			target = Point::CoordinatesToIndex(Player.Position.x, Player.Position.y + 1 * i);
			FogMap[target] = Map[target];
		}
		else
		{
			x = false;
		}

		if (c && CheckValidPoint(Player.Position.x + 1 * i, Player.Position.y + 1 * i))
		{
			target = Point::CoordinatesToIndex(Player.Position.x + 1 * i, Player.Position.y + 1 * i);
			FogMap[target] = Map[target];
		}
		else
		{
			c = false;
		}

		if (q && w)
		{
			MarkAllUnFog(Player.Position.x - 1 * i, Player.Position.y - 1 * i, Player.Position.x, Player.Position.y - 1 * i);
		}

		if (w && e)
		{
			MarkAllUnFog(Player.Position.x, Player.Position.y - 1 * i, Player.Position.x + 1 * i, Player.Position.y - 1 * i);
		}

		if (e && d)
		{
			MarkAllUnFog(Player.Position.x + 1 * i, Player.Position.y - 1 * i, Player.Position.x + 1 * i, Player.Position.y);
		}

		if (d && c)
		{
			MarkAllUnFog(Player.Position.x + 1 * i, Player.Position.y, Player.Position.x + 1 * i, Player.Position.y + 1 * i);
		}

		if (c && x)
		{
			MarkAllUnFog(Player.Position.x + 1 * i, Player.Position.y + 1 * i, Player.Position.x, Player.Position.y + 1 * i);
		}

		if (x && z)
		{
			MarkAllUnFog(Player.Position.x, Player.Position.y + 1 * i, Player.Position.x - 1 * i, Player.Position.y + 1 * i);
		}

		if (z && a)
		{
			MarkAllUnFog(Player.Position.x - 1 * i, Player.Position.y + 1 * i, Player.Position.x - 1 * i, Player.Position.y);
		}

		if (a && q)
		{
			MarkAllUnFog(Player.Position.x - 1 * i, Player.Position.y, Player.Position.x - 1 * i, Player.Position.y - 1 * i);
		}

	}
}

void Dungeon::MarkAllUnFog(int x, int y, int m, int n)
{
	int target;
	while (x != m)
	{
		target = Point::CoordinatesToIndex(x, y);
		FogMap[target] = Map[target];

		if (x > m)
			x--;
		else if (x < m)
			x++;
	}

	while (y != n)
	{
		target = Point::CoordinatesToIndex(x, y);
		FogMap[target] = Map[target];

		if (y > n)
			y--;
		else if (y < n)
			y++;
	}
}



bool Dungeon::CheckValidPoint(int x, int y)
{
	if (x < 0 || x > 79 || y > 20 || y < 0)
		return false;

	if (Map[Point::CoordinatesToIndex(x, y)] == ' ')
		return false;

	return true;
}

void Dungeon::GenerateStairs()
{
	int up = rand() % 3 + 1;
	int down = rand() % 3 + 1;


	GenerateStairsWorkers('<', up);
	GenerateStairsWorkers('>', down);
}

void Dungeon::GenerateStairsWorkers(char symbol, int count)
{
	int temp;
	int i;
	int flag = 0;

	while (count != 0)
	{
		temp = rand() % 2000 + 1;
		flag = 0;
		for (i = 0; 1; i = (i + 1) % (80 * 21))
		{
			temp--;
			if (flag)
			{
				if (MapLayout[i] == '#' || MapLayout[i] == '.')
				{
					MapLayout[i] = symbol;
					Map[i] = symbol;
					count--;
					break;
				}
			}

			if (temp <= 0)
				flag = 1;

		}
	}
}

void Dungeon::GenerateItem()
{
	if(iTemplate.size() == 0)
		Profile::ParseItemInfo(iTemplate);


	int nums = rand() % 5 + 10;
	for (int i = 0; i < nums; i++)
	{
		ItemTemplate* cur;
		while (true)
		{
			int sel = rand() % iTemplate.size();

			if (!iTemplate[sel].valid)
				continue;
			int rarity = rand() % 100;
			if (rarity < iTemplate[sel].Rarity)
				continue;
			cur = &iTemplate[sel];
			break;
		}
		Items.push_back(Item(cur));
	}
	PlaceItems();
}

void Dungeon::PlaceItems()
{

	for (int i = 0, count = 0; (size_t)i < Items.size(); i++, count++)
	{
		PlaceItemsWorker(Rooms[count%NumsRoom], Items[i]);
	}
}

void Dungeon::PlaceItemsWorker(Room &room, Item &item)
{
	do 
	{
		FindRoomRandomPoint(room, item.Position);
		
	} while (ItemMap[item.Position.ToIndex()] != NULL);

	ItemMap[item.Position.ToIndex()] = &item;
}

void Dungeon::GenerateMonster()
{
	int i;
	if(mTemplate.size() == 0)
		Profile::ParseMonsterInfo(mTemplate);

	Monsters = (NPC *)malloc(sizeof(NPC) * NumMonster);
	for (i = 0; i < NumMonster; i++)
	{
		MonsterTemplate* cur;
		while (true)
		{
			int sel = rand() % mTemplate.size();
			if (!mTemplate[sel].valid)
				continue;
			int rarity = rand() % 100;
			if (rarity < mTemplate[sel].Rarity - Level*5)
				continue;
			
			cur = &mTemplate[sel];

			break;
		}

		Monsters[i] = NPC(cur);
		Monsters[i].Dodge = 0;
		Monsters[i].Weight = 0;
		Monsters[i].Defense = 0;

		if (cur->Ability & 0b010000000)
			cur->valid = false;

		Monsters[i].LastKnownPC.x = 255;
		Monsters[i].LastKnownPC.y = 255;
	}
}

void Dungeon::PlaceAllPlayer()
{
	int exclude = GeneratePlayerPosition(Player, -1);

	Map[Player.Position.ToIndex()] = '@';

	int i;
	for (i = 0; i < NumMonster; i++)
	{
		GeneratePlayerPosition(Monsters[i], exclude);
		Map[Monsters[i].Position.ToIndex()] = Monsters[i].Ability % 16 + 65;
		Monsters[i].Symbol = Monsters[i].Ability % 16 + 65;
	}
}

int Dungeon::GeneratePlayerPosition(Character &character, int exclude)
{
	int RandomRoomPlayerIn;

	do
	{
		RandomRoomPlayerIn = rand() % NumsRoom;
		if (exclude == RandomRoomPlayerIn)
			continue;

		FindRoomRandomPoint(Rooms[RandomRoomPlayerIn], character.Position);
	} while (Map[character.Position.ToIndex()] != '.');

	return RandomRoomPlayerIn;
}


