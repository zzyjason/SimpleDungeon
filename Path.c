#include <stdlib.h>
#include <stdio.h>
#include "DungeonGenerator.h"
#include "Path.h"
#include "main.h"
#include "ProfileManager.h"
#include <unistd.h>


void UpdatePath(MapInfo *mapInfo)
{
	PathWalker(mapInfo, mapInfo->playerPosition, mapInfo->playerPosition, 0);
	PathWalker(mapInfo, mapInfo->playerPosition, mapInfo->playerPosition, 1);
	printPath(mapInfo);
}

void PathWalker(MapInfo *mapInfo, Point Des, Point Src, int Tunnel)
{
	if (!PathChecker(mapInfo, Des, Tunnel))
		return;
	
	char flag = 0;
	int srcIndex = PointToIndex(&Src);
	int desIndex = PointToIndex(&Des);


	if (Src.x == Des.x && Src.y == Des.y)
	{
		if (Tunnel)
		{
			mapInfo->TunnelPath.Distance[desIndex] = 0;
		}
		else
		{
			mapInfo->nonTunnelPath.Distance[desIndex] = 0;
		}
		flag = 1;
	}
	else
	{
		if (Tunnel)
		{
			
			if (mapInfo->TunnelPath.Distance[desIndex] > mapInfo->TunnelPath.Distance[srcIndex] + 1 + ((int)mapInfo->hardness[desIndex] / 85))
			{
				mapInfo->TunnelPath.Distance[desIndex] = mapInfo->TunnelPath.Distance[srcIndex] + 1  + ((int)mapInfo->hardness[desIndex] / 85);
				mapInfo->TunnelPath.ShortestPath[desIndex] = Src;
				flag = 1;
			}
			
		}
		else
		{
		
			if (mapInfo->nonTunnelPath.Distance[desIndex] > mapInfo->nonTunnelPath.Distance[srcIndex] + 1)
			{
				mapInfo->nonTunnelPath.Distance[desIndex] = mapInfo->nonTunnelPath.Distance[srcIndex] + 1;
				mapInfo->nonTunnelPath.ShortestPath[desIndex] = Src;
				flag = 1;
			}
		}
	}


	if (flag)
	{
		Src = Des;
		Des.x++;//Right
		PathWalker(mapInfo, Des, Src, Tunnel);
		Des.y++;//DownRight
		PathWalker(mapInfo, Des, Src, Tunnel);
		Des.x--;//Bottom
		PathWalker(mapInfo, Des, Src, Tunnel);
		Des.x--;//BottomLeft
		PathWalker(mapInfo, Des, Src, Tunnel);
		Des.y--;//Left
		PathWalker(mapInfo, Des, Src, Tunnel);
		Des.y--;//LeftTop
		PathWalker(mapInfo, Des, Src, Tunnel);
		Des.x++;//Top
		PathWalker(mapInfo, Des, Src, Tunnel);
		Des.x++;//TopRight
		PathWalker(mapInfo, Des, Src, Tunnel);
	}
}


int PathChecker(MapInfo *mapInfo, Point position, int Tunnel)
{
	if (position.x < 1 || position.x >78)
		return 0;
	if (position.y < 1 || position.y >19)
		return 0;

	if (Tunnel)
	{
		return 1;
	}

	switch (mapInfo->map[PointToIndex(&position)])
	{
	case ' ':
		return 0;
	case '@':
		return 1;
	case '.':
		return 1;
	case '#':
		return 1;
	default:
		return 0;
	}
	return 0;

}

void printPath(MapInfo *mapInfo)
{
	int i = 0,j;
	printf("----------------------------------------------------------------------------------\n");
	for (i = 0; i<21; i++)
	{
		printf("|");
		for (j = 0; j<80; j++)
		{

			if (mapInfo->nonTunnelPath.Distance[CoordinatesToIndex(j, i)] == 65535)
			{
				printf("%c", ' ');
			}
			else
				printf("%d", mapInfo->nonTunnelPath.Distance[CoordinatesToIndex(j, i)]%10);
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------------------\n");

	printf("----------------------------------------------------------------------------------\n");
	for (i = 0; i<21; i++)
	{
		printf("|");
		for (j = 0; j<80; j++)
		{
			if (mapInfo->TunnelPath.Distance[CoordinatesToIndex(j, i)] == 65535)
			{
				printf("%c", ' ');
			}
			else
				printf("%d", mapInfo->TunnelPath.Distance[CoordinatesToIndex(j, i)] % 10);
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------------------\n");

}



