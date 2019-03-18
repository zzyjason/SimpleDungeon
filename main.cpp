#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <vector>

#include "Dungeon.h"
#include "Profile.h"
#include "Turn.h"
#include <iostream>

int main(int argc, char **argv)
{
	srand(time(NULL));
	int loadFlag = 0;
	int saveFlag = 0;
	int i, j;
	int numMon = rand() % 5 + 8;

	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--save") == 0)
			saveFlag = 1;
		else if (strcmp(argv[i], "--load") == 0)
			loadFlag = 1;
		else if (strcmp(argv[i], "--nummon") == 0 && i + 1 < argc)
		{
			numMon = 0;
			for (j = 0; j < (int)strlen(argv[i + 1]); j++)
			{
				if (argv[i + 1][j] > 47 && argv[i + 1][j] < 58)
					numMon = numMon * 10 + argv[i + 1][j] - 48;
				else
				{
					numMon = rand() % 5 + 8;
					break;
				}
			}
		}
	}

	Dungeon dungeon;

	if (loadFlag)
	{
		Profile::LoadProfile(dungeon);
	}
	else
	{
		dungeon.GenerateNew();
		dungeon.GenerateStairs();
	}

	

	if (saveFlag)
		Profile::SaveProfile(dungeon);

	dungeon.GenerateItem();

	dungeon.NumMonster = numMon;
	dungeon.GenerateMonster();
	dungeon.PlaceAllPlayer();

	dungeon.Tunnel.Construct(true, dungeon);
	dungeon.NonTunnel.Construct(true, dungeon);
	int Level = 0;
	Turn turn(dungeon.NumMonster, dungeon, &Level);
	WINDOW* scr = initscr();
	start_color();
	curs_set(0);
	
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_WHITE);

	int result;

	dungeon.Level = 0;
	do
	{
		result = turn.NextTurn();
		if (result != 0)
			continue;

		while (turn.Queue.peak() != NULL)
			turn.Queue.pop();

		free(dungeon.Rooms);
		free(dungeon.Monsters);
		free(turn.Queue.data);

		dungeon = Dungeon();

		dungeon.GenerateNew();
		dungeon.Level = Level;

		dungeon.GenerateStairs();
		dungeon.GenerateItem();
		dungeon.NumMonster = numMon;
		dungeon.GenerateMonster();
		dungeon.PlaceAllPlayer();
		dungeon.Tunnel.Construct(true, dungeon);
		dungeon.NonTunnel.Construct(true, dungeon);

		turn = Turn(dungeon.NumMonster, dungeon, &Level);

	} while (result >= 0);

	switch (result)
	{
	case -1:
		mvprintw(23, 0, "Dead");
		break;

	case -2:
		mvprintw(23, 0, "Quit");
		break;
	}

	refresh();
	getch();

	endwin();
	delwin(scr);

	free(dungeon.Rooms);
	free(dungeon.Monsters);
	free(turn.Queue.data);

	

	return 0;

}