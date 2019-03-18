#include "Util.h"
#include "Dungeon.h"
#include "Path.h"
#include <stdlib.h>
#include <iostream>


Path::Path(bool tunnle, Dungeon dungeon)
{
	Construct(tunnle, dungeon);
}

Path::Path()
{

}


void Path::Construct(bool tunnle, Dungeon dungeon)
{
	int i;
	for (i = 0; i< HEIGHT * WIDTH; i++)
	{
		Distance[i] = 65535;
	}

	PathWalker(dungeon, dungeon.Player.Position, dungeon.Player.Position, tunnle);

	if (!tunnle)
		return;

	for (i = 0; i < 80 * 21; i++)
	{
		if (Distance[i] != 65535 && Distance[i] != 0)
		{
			Distance[i] -= dungeon.Hardness[i] / 85;
		}
	}
}

void Path::PathWalker(Dungeon &dungeon, Point des, Point src, bool tunnel)
{
	static int stack = 0;
	stack++;

	if (!PathChecker(dungeon, des, tunnel))
		return;

	char flag = 0;
	int srcIndex = src.ToIndex();
	int desIndex = des.ToIndex();


	if (src.x == des.x && src.y == des.y)
	{
		if (tunnel)
		{
			Distance[desIndex] = 0;
		}
		else
		{
			Distance[desIndex] = 0;
		}
		flag = 1;
	}
	else
	{
		if (tunnel)
		{

			if (Distance[desIndex] > Distance[srcIndex] + 1 + ((int)dungeon.Hardness[desIndex] / 85))
			{
				Distance[desIndex] = Distance[srcIndex] + 1 + ((int)dungeon.Hardness[desIndex] / 85);
				ShortestPath[desIndex].x = src.x;
				ShortestPath[desIndex].y = src.y;
				flag = 1;
			}
		}
		else
		{

			if (Distance[desIndex] > Distance[srcIndex] + 1)
			{
				Distance[desIndex] = Distance[srcIndex] + 1;
				ShortestPath[desIndex].x = src.x;
				ShortestPath[desIndex].y = src.y;
				flag = 1;
			}
		}
	}


	if (flag == 1)
	{
		src.x = des.x;
		src.y = des.y;
		des.x++;//Right
		PathWalker(dungeon, des, src, tunnel);
		des.y++;//DownRight
		PathWalker(dungeon, des, src, tunnel);
		des.x--;//Bottom
		PathWalker(dungeon, des, src, tunnel);
		des.x--;//BottomLeft
		PathWalker(dungeon, des, src, tunnel);
		des.y--;//Left
		PathWalker(dungeon, des, src, tunnel);
		des.y--;//LeftTop
		PathWalker(dungeon, des, src, tunnel);
		des.x++;//Top
		PathWalker(dungeon, des, src, tunnel);
		des.x++;//TopRight
		PathWalker(dungeon, des, src, tunnel);
	}

	stack--;
	//std::cout << "Out " << stack << std::endl;
}

bool Path::PathChecker(Dungeon &dungeon, Point position, bool tunnel)
{
	if (position.x < 1 || position.x >78)
		return false;
	if (position.y < 1 || position.y >19)
		return false;

	if (tunnel)
	{
		return true;
	}


	if (dungeon.Hardness[position.ToIndex()] == 0)
		return true;

	return false;
}
