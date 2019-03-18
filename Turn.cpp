#include "Turn.h"
#include "Dungeon.h"
#include <stdlib.h>
#include <string.h>
#include <climits>


Turn::Turn(int maxChar, Dungeon &dungeon, int* level)
	:Queue(maxChar, (int(*)(void*, void*))(&ComparePC))
{
	DungeonInfo = dungeon;
	Level = level;
	dungeon.Player.NextRound = 1000 / dungeon.Player.Speed;
	Queue.push((void*)&dungeon.Player);

	int i;
	for (i = 0; i < dungeon.NumMonster; i++)
	{
		dungeon.Monsters[i].NextRound = 1000 / dungeon.Monsters[i].Speed;
		Queue.push((void*)&dungeon.Monsters[i]);
	}
	
}

int Turn::ComparePC(const Character* a, const Character* b)
{
	if (a->NextRound > b->NextRound)
		return 1;
	return 0;
}

int Turn::NextTurn()
{
	Character* Next = (Character*)Queue.peak();

	if (Next == NULL)
		return 0;

	while (!Next->Alive)
	{
		Queue.pop();
		
		Next = (Character*)Queue.peak();

		if (Next == NULL)
			return 0;
	}

	int turn = Next->NextRound;
	int result;

	NPC* npc = (NPC*)(Next);

	if (!Next->User)
		result = NPCAction(*npc);
	else
		result = UserAction();
	
	if (result < 0)
		return result;

	if (result == 3 || result == 4)
	{
		if (result == 3)
			(*Level)++;
		else
			(*Level)--;

		if (*Level < 0)
			*Level = 0;

		return 0;
	}
	int speed = (Next->Speed - Next->Weight);
	if (speed < 5)
		speed = 5;

	Next->NextRound = turn + 1000 / (speed);
	Queue.heapifyDown(0);

	return turn;
}

void Turn::DisplayUI(bool fog)
{
	if (fog)
	{
		DungeonInfo.UpdateFogHallway();
		DungeonInfo.PrintFogHallway();
	}
	else
		DungeonInfo.PrintHallway();

	mvprintw(21,0, "Hp: %d	Speed: %d	Damage: %d	Level: %d", DungeonInfo.Player.HP, DungeonInfo.Player.Speed, DungeonInfo.Player.Damage, DungeonInfo.Level);

	int count = 5;
	mvprintw(22, 0, "Bag: ");
	
	for (int i = 0; i < 10; i++)
	{
		mvaddch(22, count++, '[');
		if (DungeonInfo.Player.Bag[i] != NULL)
		{
			attron(COLOR_PAIR(DungeonInfo.Player.Bag[i]->Color));
			mvaddch(22, count++, DungeonInfo.Player.Bag[i]->Symbol);
			attroff(COLOR_PAIR(DungeonInfo.Player.Bag[i]->Color));
		}
		else
		{
			mvaddch(22, count++, ' ');
		}
		mvaddch(22, count++, ']');
		mvaddch(22, count++, '	');
	}

	

}

int Turn::SelectItem()
{
	WINDOW *info = NULL;
	info = newwin(15, 60, 3, 10);
	wborder(info, '|', '|', '-', '-', '+', '+', '+', '+');
	DisplayItems(info);
	int result = 1;
	wmove(info, result + 1, 2);
	wprintw(info, "->");
	wrefresh(info);

	while (true)
	{
		flushinp();
		char input = wgetch(info);

		if (input == 10)
			break;

		if (input != 27)
			continue;

		nodelay(info, 1);
		wgetch(info);
		input = wgetch(info);

		if (input == 'A')
		{
			if (result != 1)
			{
				wmove(info, result + 1, 2);
				wprintw(info, "  ");
				result--;

				wmove(info, result + 1, 2);
				wprintw(info, "->");
				wrefresh(info);
			}
		}
		else if (input == 'B')
		{
			if (result != 10)
			{
				wmove(info, result + 1, 2);
				wprintw(info, "  ");
				result++;

				wmove(info, result + 1, 2);
				wprintw(info, "->");
				wrefresh(info);
			}
		}
		else if (input == ERR)
		{
			result = -1;
			break;
		}

		nodelay(info, 0);
	}

	delwin(info);
	return result;
}

void Turn::DisplayItems(WINDOW *window)
{
	wmove(window, 1, 2);
	wprintw(window, "Bag Info");
	for (int i = 0; i < 10; i++)
	{
		wmove(window, i + 2, 4);
		wprintw(window,"Item %d	[", i + 1);
		if (DungeonInfo.Player.Bag[i] != NULL)
		{
			wattron(window,COLOR_PAIR(DungeonInfo.Player.Bag[i]->Color));
			wprintw(window, "%c", DungeonInfo.Player.Bag[i]->Symbol);
			wattroff(window,COLOR_PAIR(DungeonInfo.Player.Bag[i]->Color));
			wprintw(window ,"]: %s", DungeonInfo.Player.Bag[i]->Template->Name.c_str());
		}
		else
		{
			wprintw(window, " ]");
		}
	}
	wrefresh(window);
}

void Turn::DisplayEquipment(WINDOW* window)
{
	wmove(window, 1, 2);
	wprintw(window, "Equipment");
	
	int count = 2;

	wmove(window, count++, 3);
	wprintw(window, "Weapon [");
	Item* cur = DungeonInfo.Player.Weapon[0];
	EquipmentDisplayWorker(window, cur);


	wmove(window, count++, 3);
	wprintw(window, "OFFHAND [");
	cur = DungeonInfo.Player.Weapon[1];
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "RANGED [");
	cur = DungeonInfo.Player.Ranged;
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "ARMOR [");
	cur = DungeonInfo.Player.Armor;
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "ARMOR [");
	cur = DungeonInfo.Player.Ranged;
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "HELMET [");
	cur = DungeonInfo.Player.Helmet;
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "CLOAK [");
	cur = DungeonInfo.Player.Cloak;
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "GLOVES [");
	cur = DungeonInfo.Player.Gloves;
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "Boots [");
	cur = DungeonInfo.Player.Boots;
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "Ring [");
	cur = DungeonInfo.Player.Rings[0];
	EquipmentDisplayWorker(window, cur);

	wmove(window, count++, 3);
	wprintw(window, "Ring [");
	cur = DungeonInfo.Player.Rings[1];
	EquipmentDisplayWorker(window, cur);

	wrefresh(window);
}

void Turn::EquipmentDisplayWorker(WINDOW* window, Item* cur)
{
	if (cur != NULL)
	{
		wattron(window, COLOR_PAIR(cur->Color));
		wprintw(window, "%c", cur->Symbol);
		wattroff(window, COLOR_PAIR(cur->Color));
		wprintw(window, "]: %s", cur->Template->Name.c_str());
	}
	else
	{
		wprintw(window, " ]");
	}
}

void Turn::DisplayMonsterWindowInfo(WINDOW* window, NPC* monster)
{
	wmove(window, 1, 2);
	wprintw(window, "Monster Info");

	int count = 2;
	int val;
	wmove(window, count++, 3);
	wprintw(window, "Name: %s", monster->Template->Name.c_str());

	wmove(window, count++, 3);
	wprintw(window, "Description: %s", monster->Template->Description.c_str());
	count += 7;

	wmove(window, count++, 3);
	val = monster->HP;
	wprintw(window, "HP: %d", val);

	wmove(window, count++, 3);
	val = monster->Damage;
	wprintw(window, "Damage: %d", val);

	wmove(window, count++, 3);
	val = monster->Speed;
	wprintw(window, "Speed: %d", val);

	wrefresh(window);
}

void Turn::DisplayItemInfo(WINDOW* window, Item* item)
{
	wmove(window, 1, 2);
	wprintw(window, "Item Info");
	
	int count = 2;
	wmove(window, count++, 3);
	wprintw(window, "Name: %s", item->Template->Name.c_str());

	wmove(window, count++, 3);
	wprintw(window, "Description: %s", item->Template->Description.c_str());

	count += 2;
	int val = 0;

	wmove(window, count++, 3);
	wprintw(window, "Type: %s", item->Template->Type.c_str());

	wmove(window, count++, 3);
	val = item->Hit;
	wprintw(window, "Hit: %d", val);

	wmove(window, count++, 3);
	val = item->Damage;
	wprintw(window, "Damage: %d", val);

	wmove(window, count++, 3);
	val = item->Dodge;
	wprintw(window, "Dodge: %d", val);

	wmove(window, count++, 3);
	val = item->Def;
	wprintw(window, "Defense: %d", val);

	wmove(window, count++, 3);
	val = item->Weight;
	wprintw(window, "Weight: %d", val);

	wmove(window, count++, 3);
	val = item->Speed;
	wprintw(window, "Speed: %d", val);

	wmove(window, count++, 3);
	val = item->Attribute;
	wprintw(window, "Attribute: %d", val);

	wmove(window, count++, 3);
	val = item->Template->Rarity;
	wprintw(window, "Rarity: %d", val);

	wrefresh(window);
}



int Turn::UserAction()
{
	flushinp();
	char input;
	Point nextMove;
	WINDOW *info = NULL;


	static bool Fog = true;
	Point point;
	char lastChar;
	bool tp = false;
	bool lo = false;
	DisplayUI(Fog);
	
	do
	{
		nextMove.x = 255;
		nextMove.y = 255;
		noecho();
		input = getch();

		if (input == '7' || input == 'y')
		{
			if (tp)
			{
				if (validateTP(point.x - 1, point.y - 1))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.x--;
					point.y--;
				}
			}
			else
			{
				nextMove.x = DungeonInfo.Player.Position.x - 1;
				nextMove.y = DungeonInfo.Player.Position.y - 1;

			}

		}
		else if (input == '8' || input == 'k')
		{
			if (tp)
			{
				if (validateTP(point.x, point.y - 1))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.y--;

				}
			}
			else
			{
				nextMove.x = DungeonInfo.Player.Position.x;
				nextMove.y = DungeonInfo.Player.Position.y - 1;


			}

		}
		else if (input == '9' || input == 'u')
		{
			if (tp)
			{

				if (validateTP(point.x + 1, point.y - 1))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.x++;
					point.y--;

				}
			}
			else
			{
				nextMove.x = DungeonInfo.Player.Position.x + 1;
				nextMove.y = DungeonInfo.Player.Position.y - 1;


			}

		}
		else if (input == '6' || input == 'l')
		{
			if (tp)
			{
				if (validateTP(point.x + 1, point.y))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.x++;

				}

			}
			else
			{
				nextMove.x = DungeonInfo.Player.Position.x + 1;
				nextMove.y = DungeonInfo.Player.Position.y;


			}

		}
		else if (input == '3' || input == 'n')
		{
			if (tp)
			{
				if (validateTP(point.x + 1, point.y + 1))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.x++;
					point.y++;

				}
			}
			else
			{
				nextMove.x = DungeonInfo.Player.Position.x + 1;
				nextMove.y = DungeonInfo.Player.Position.y + 1;


			}
		}
		else if (input == '2' || input == 'j')
		{
			if (tp)
			{
				if (validateTP(point.x, point.y + 1))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.y++;

				}
			}
			else
			{

				nextMove.x = DungeonInfo.Player.Position.x;
				nextMove.y = DungeonInfo.Player.Position.y + 1;

			}
		}
		else if (input == '1' || input == 'b')
		{
			if (tp)
			{
				if (validateTP(point.x - 1, point.y + 1))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.x--;
					point.y++;

				}
			}
			else
			{
				nextMove.x = DungeonInfo.Player.Position.x - 1;
				nextMove.y = DungeonInfo.Player.Position.y + 1;


			}

		}
		else if (input == '4' || input == 'h')
		{
			if (tp)
			{
				if (validateTP(point.x - 1, point.y))
				{
					DungeonInfo.Map[point.ToIndex()] = lastChar;
					point.x--;

				}
			}
			else
			{
				nextMove.x = DungeonInfo.Player.Position.x - 1;
				nextMove.y = DungeonInfo.Player.Position.y;


			}

		}
		else if (input == '5' || input == ' ')
		{
			if (tp)
				continue;
			nextMove.x = DungeonInfo.Player.Position.x;
			nextMove.y = DungeonInfo.Player.Position.y;
		}
		else if (input == '>')
		{
			if (DungeonInfo.MapLayout[DungeonInfo.Player.Position.ToIndex()] == '>')
				return 3;
		}
		else if (input == '<')
		{
			if (DungeonInfo.MapLayout[DungeonInfo.Player.Position.ToIndex()] == '<')
				return 4;
		}
		else if (input == 'm')
		{
			if (info == NULL)
			{
				info = newwin(15, 60, 3, 10);
				wborder(info, '|', '|', '-', '-', '+', '+', '+', '+');
				DisplayMonsterInfo(info, 0);
			}
		}
		else if (input == 27)
		{
			if (info != NULL)
			{
				nodelay(info, 1);
				wgetch(info);
				switch (wgetch(info))
				{
				case 'A':
					DisplayMonsterInfo(info, -1);
					break;

				case 'B':

					DisplayMonsterInfo(info, 1);
					break;

				case ERR:

					delwin(info);
					refresh();
					info = NULL;

					if (Fog)
					{
						DungeonInfo.UpdateFogHallway();
						DungeonInfo.PrintFogHallway();
					}
					else
						DungeonInfo.PrintHallway();
					break;
				}
				nodelay(info, 0);
				flushinp();
			}
		}
		else if (input == 'q')
		{
			return -2;
		}
		else if (input == 'f')
		{
			if (!Fog)
			{
				Fog = true;
				DungeonInfo.UpdateFogHallway();
				DungeonInfo.PrintFogHallway();
			}
			else
			{
				Fog = false;
				DungeonInfo.PrintHallway();
			}
		}
		else if (input == 'g')
		{
			if (tp)
			{
				tp = false;
				DungeonInfo.Map[point.ToIndex()] = '@';
				DungeonInfo.Map[DungeonInfo.Player.Position.ToIndex()] = DungeonInfo.MapLayout[DungeonInfo.Player.Position.ToIndex()];

				KillCharacter(point);
				DungeonInfo.Player.Position.x = point.x;
				DungeonInfo.Player.Position.y = point.y;
				DungeonInfo.Map[DungeonInfo.Player.Position.ToIndex()] = '@';
				

				if (Fog)
				{
					DungeonInfo.UpdateFogHallway();
					DungeonInfo.PrintFogHallway();
				}
				else
					DungeonInfo.PrintHallway();

			}
			else
			{
				tp = true;

				DungeonInfo.PrintHallway();
				DungeonInfo.FogMap[DungeonInfo.Player.Position.ToIndex()] = DungeonInfo.MapLayout[DungeonInfo.Player.Position.ToIndex()];
				lastChar = DungeonInfo.MapLayout[DungeonInfo.Player.Position.ToIndex()];
				DungeonInfo.Map[DungeonInfo.Player.Position.ToIndex()] = '*';
				point.x = DungeonInfo.Player.Position.x;
				point.y = DungeonInfo.Player.Position.y;
			}

		}
		else if (input == 'r')
		{
			tp = false;
			DungeonInfo.Map[point.ToIndex()] = lastChar;
			point.x = rand() % 78 + 1;
			point.y = rand() % 19 + 1;

			DungeonInfo.Map[point.ToIndex()] = '@';
			DungeonInfo.Map[DungeonInfo.Player.Position.ToIndex()] = DungeonInfo.MapLayout[DungeonInfo.Player.Position.ToIndex()];

			KillCharacter(point);
			DungeonInfo.Player.Position.x = point.x;
			DungeonInfo.Player.Position.y = point.y;



			if (Fog)
			{
				DungeonInfo.UpdateFogHallway();
				DungeonInfo.PrintFogHallway();
			}
			else
				DungeonInfo.PrintHallway();
		}
		else if (input == ',')
		{
			if (DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()] != NULL)
			{
				bool success = false;
				EquipItem(DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()], false);
				for (int i = 0; i < 10; i++)
				{
					if (DungeonInfo.Player.Bag[i] == NULL)
					{
						success = true;
						DungeonInfo.Player.Bag[i] = DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()];
						DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()] = NULL;
						
						break;
					}
				}
				if (!success)
				{
					Item *temp = DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()];
					EquipItem(DungeonInfo.Player.Bag[9], true);
					DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()] = DungeonInfo.Player.Bag[9];
					DungeonInfo.Player.Bag[9] = temp;

				}
				
				DisplayUI(Fog);
			}
		}
		else if (input == 'i')
		{
			SelectItem();
			DisplayUI(Fog);
		}
		else if (input == 'd')
		{
			int drop = SelectItem();
			if (drop != -1 && DungeonInfo.Player.Bag[drop-1] != NULL)
			{
				if (DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()] == NULL)
				{
					DungeonInfo.ItemMap[DungeonInfo.Player.Position.ToIndex()] = DungeonInfo.Player.Bag[drop-1];
					DungeonInfo.Player.Bag[drop-1] = NULL;
				}
			}
			DisplayUI(Fog);
		}
		else if (input == 'x')
		{
			int drop = SelectItem();
			if (drop != -1 && DungeonInfo.Player.Bag[drop - 1] != NULL)
			{
				DungeonInfo.Player.Bag[drop - 1] = NULL;
			}
			DisplayUI(Fog);
		}
		else if (input == 'w')
		{
			int wear = SelectItem();
			if (wear != -1 && DungeonInfo.Player.Bag[wear - 1] != NULL)
			{
				Item* temp;
				switch (DungeonInfo.Player.Bag[wear - 1]->Symbol)
				{
					case '|':
						if (DungeonInfo.Player.Weapon[0] == NULL)
						{
							DungeonInfo.Player.Weapon[0] = DungeonInfo.Player.Bag[wear - 1];
							DungeonInfo.Player.Bag[wear - 1] = NULL;
						}
						else if (DungeonInfo.Player.Weapon[0]->Symbol == '|')
						{
							temp = DungeonInfo.Player.Weapon[0];
							DungeonInfo.Player.Weapon[0] = DungeonInfo.Player.Bag[wear - 1];
							DungeonInfo.Player.Bag[wear - 1] = temp;
						}
						break;
					case ')':
						if (DungeonInfo.Player.Weapon[1] == NULL)
						{
							DungeonInfo.Player.Weapon[1] = DungeonInfo.Player.Bag[wear - 1];
							DungeonInfo.Player.Bag[wear - 1] = NULL;
						}
						else if (DungeonInfo.Player.Weapon[1]->Symbol == ')')
						{
							temp = DungeonInfo.Player.Weapon[1];
							DungeonInfo.Player.Weapon[1] = DungeonInfo.Player.Bag[wear - 1];
							DungeonInfo.Player.Bag[wear - 1] = temp;
						}
						break;
					case '}':
						temp = DungeonInfo.Player.Ranged;
						DungeonInfo.Player.Ranged = DungeonInfo.Player.Bag[wear - 1];
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					case '[':
						temp = DungeonInfo.Player.Armor;
						DungeonInfo.Player.Armor = DungeonInfo.Player.Bag[wear - 1];
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					case ']':
						temp = DungeonInfo.Player.Helmet;
						DungeonInfo.Player.Helmet = DungeonInfo.Player.Bag[wear - 1];
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					case '(':
						temp = DungeonInfo.Player.Cloak;
						DungeonInfo.Player.Cloak = DungeonInfo.Player.Bag[wear - 1];
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					case '{':
						temp = DungeonInfo.Player.Gloves;
						DungeonInfo.Player.Gloves = DungeonInfo.Player.Bag[wear - 1];
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					case '\\':
						temp = DungeonInfo.Player.Boots;
						DungeonInfo.Player.Boots = DungeonInfo.Player.Bag[wear - 1];
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					case '=':
						if (DungeonInfo.Player.Rings[0] == NULL)
						{
							temp = DungeonInfo.Player.Rings[0];
							DungeonInfo.Player.Rings[0] = DungeonInfo.Player.Bag[wear - 1];
						}
						else
						{
							temp = DungeonInfo.Player.Rings[1];
							DungeonInfo.Player.Rings[1] = DungeonInfo.Player.Bag[wear - 1];
						}
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					case '_':
						temp = DungeonInfo.Player.Light;
						DungeonInfo.Player.Light = DungeonInfo.Player.Bag[wear - 1];
						DungeonInfo.Player.Bag[wear - 1] = temp;
						break;
					default:
						break;
				}
			}
			DisplayUI(Fog);
		}
		else if (input == 'e')
		{
			WINDOW* eq = newwin(15, 60, 3, 10);
			wborder(eq, '|', '|', '-', '-', '+', '+', '+', '+');
			DisplayEquipment(eq);
			getch();
			delwin(eq);
			DisplayUI(Fog);

		}

		else if (input == 'I')
		{
			int item = SelectItem();
			if (item != -1 && DungeonInfo.Player.Bag[item - 1] != NULL)
			{
				WINDOW* it = newwin(18, 80, 3, 0);
				wborder(it, '|', '|', '-', '-', '+', '+', '+', '+');
				DisplayItemInfo(it, DungeonInfo.Player.Bag[item - 1]);
				getch();
				delwin(it);
			}
			DisplayUI(Fog);
		}

		else if (input == 'L')
		{
			Point selected = SelectSpot(Fog);
			if (selected.x < 1 || selected.x > 78 || selected.y > 19 || selected.y < 1)
				continue;
			NPC* target = NULL;
			for (int i = 0; i < DungeonInfo.NumMonster; i++)
			{
				if (DungeonInfo.Monsters[i].Position == selected && DungeonInfo.Monsters[i].Alive)
				{
					target = &DungeonInfo.Monsters[i];
					break;
				}
			}
			if (target != NULL)
			{
				WINDOW* it = newwin(20, 80, 0, 0);
				wborder(it, '|', '|', '-', '-', '+', '+', '+', '+');
				DisplayMonsterWindowInfo(it, target);
				flushinp();
				wgetch(it);
				delwin(it);
			}
			DisplayUI(Fog);

		}


		if (tp || lo)
		{
			if(DungeonInfo.Map[point.ToIndex()] != '*')
				lastChar = DungeonInfo.Map[point.ToIndex()];
			DungeonInfo.Map[point.ToIndex()] = '*';
			if (info == NULL)
				DungeonInfo.PrintHallway();
		}

	} while (!MovePC(nextMove, DungeonInfo.Player));

	DungeonInfo.Tunnel.Construct(true, DungeonInfo);
	DungeonInfo.NonTunnel.Construct(true, DungeonInfo);

	if (info != NULL)
		delwin(info);

	return 0;
}

void Turn::EquipItem(Item* item, bool DeEquip)
{
	int factor = 1;
	if (DeEquip)
		factor = -1;

	DungeonInfo.Player.Defense += factor * item->Def;
	DungeonInfo.Player.Weight += factor * item->Weight;
	DungeonInfo.Player.Dodge += factor * item->Weight;

}


Point Turn::SelectSpot(bool fog)
{
	Point result;
	result.x = DungeonInfo.Player.Position.x;
	result.y = DungeonInfo.Player.Position.y;
	char input;
	while (true)
	{
		DisplayUI(fog);
		mvprintw(result.y, result.x, "*");
		
		input = getch();

		if (input == '7' || input == 'y')
		{
			result.x -= 1;
			result.y -= 1;
		}
		else if (input == '8' || input == 'k')
		{
			result.y -= 1;
		}
		else if (input == '9' || input == 'u')
		{
			result.x += 1;
			result.y -= 1;
		}
		else if (input == '6' || input == 'l')
		{
			result.x += 1;
		}
		else if (input == '3' || input == 'n')
		{
			result.x += 1;
			result.y += 1;

		}
		else if (input == '2' || input == 'j')
		{
			result.y += 1;
		}
		else if (input == '1' || input == 'b')
		{
			result.x -= 1;
			result.y += 1;
		}
		else if (input == '4' || input == 'h')
		{

			result.x -= 1;
		}

		if (input == 27 || input == 10)
		{
			break;
		}
	}
	DisplayUI(fog);

	return result;
}

bool Turn::validateTP(int x, int y)
{
	if (x < 1 || x > 78 || y > 19 || y < 1)
		return false;
	return true;
}

const int TELE =	0b00100;
const int SMART =	0b01000;
const int TUNNEL =	0b00010;
const int ERRATIC = 0b00001;
const int PASS =	0b010000;
const int PICKUP =	0b0100000;
const int DESTROY = 0b01000000;
const int UNIQ =	0b010000000;
const int BOSS =	0b0100000000;

int Turn::NPCAction(NPC &npc)
{
	int random = 0;
	Point Now = npc.Position;
	Point NextMove = Now;
	

	if (npc.Ability & 0b00100 || CheckLineOfSight(npc.Position))
	{
		npc.LastKnownPC = npc.Position;
	}
	else
		random = 1;


	if (npc.Ability & 0b01000)
	{
		if (npc.LastKnownPC.x == 255 || npc.LastKnownPC.y == 255)
			random = 1;
		else
		{
			if (npc.Ability & 0b00010)
			{
				NextMove = DungeonInfo.Tunnel.ShortestPath[Now.ToIndex()];
			}
			else
			{
				NextMove = DungeonInfo.NonTunnel.ShortestPath[Now.ToIndex()];
			}
		}


		if (npc.LastKnownPC == npc.Position)
		{
			npc.LastKnownPC.x = 255;
			npc.LastKnownPC.y = 255;
		}
	}
	else
	{
		if (!CheckLineOfSight(npc.Position))
		{
			random = 1;
		}
		else
		{
			if (npc.Position.x > DungeonInfo.Player.Position.x)
				NextMove.x--;
			if (npc.Position.x < DungeonInfo.Player.Position.x)
				NextMove.x++;


			if (npc.Position.y > DungeonInfo.Player.Position.y)
				NextMove.y--;
			if (npc.Position.y < DungeonInfo.Player.Position.y)
				NextMove.y++;
		}

	}

	if ((npc.Ability & 0b00001 && rand() % 2) || random)
	{
		do
		{
			NextMove = RandomMove(npc.Position);

		} while (!ValidateNextMove(NextMove, (npc.Ability & 0b00010)));
	}


	return MovePC(NextMove, npc);
}

int Turn::ValidateNextMove(Point &nextMove, bool tunnle)
{
	if (nextMove.x < 1 || nextMove.x > 78 || nextMove.y > 19 || nextMove.y < 1)
		return 0;

	unsigned char hardness = DungeonInfo.Hardness[nextMove.ToIndex()];
	if (tunnle)
	{
		if (hardness == 255)
			return 0;
		else if (hardness <= 85)
			return 1;
		else
			return 2;
	}
	else if (hardness > 0)
		return 0;

	return 1;
}

int Turn::MovePC(Point &nextMove, Character &character)
{
	
	int flag;
	if (!character.User)
		flag = ValidateNextMove(nextMove, (*(NPC*)&character).Ability & 0b0010);
	else
		flag = ValidateNextMove(nextMove, false);

	if (flag == 0)
		return 0;
	
	DungeonInfo.Player.HP += DungeonInfo.Player.MaxHP * 0.001;
	if (DungeonInfo.Player.HP > DungeonInfo.Player.MaxHP)
		DungeonInfo.Player.HP = DungeonInfo.Player.MaxHP;

	if (flag == 1)
	{
		DungeonInfo.Hardness[nextMove.ToIndex()] = 0;

		if (DungeonInfo.MapLayout[nextMove.ToIndex()] == ' ')
			DungeonInfo.MapLayout[nextMove.ToIndex()] = '#';


		if (DungeonInfo.Map[nextMove.ToIndex()] != DungeonInfo.MapLayout[nextMove.ToIndex()])
		{
			
			Character* target = NULL;
			if (DungeonInfo.Player.Position == nextMove)
				target = &DungeonInfo.Player;

			for (int i = 0; i < DungeonInfo.NumMonster; i++)
			{
				if (DungeonInfo.Monsters[i].Position == nextMove)
				{
					target = &DungeonInfo.Monsters[i];
					break;
				}
			}

			if (!character.User && (target == NULL || !target->User))
				return 1;
				

			if (character.GetDamage() >= target->HP)
			{
				if (KillCharacter(nextMove) == -1)
					return -1;
			}
			else
			{
				if (rand() % 100 < target->Dodge)
					return 1;

				int damage = character.GetDamage() - target->Defense;


				if (character.User)
					damage = ((PC&)character).GetDamage() - ((NPC*)target)->Defense;



				if (damage <= 0)
					return 1;

				target->HP -= damage;
				
				return 1;
			}

		}

		DungeonInfo.Map[character.Position.ToIndex()] = DungeonInfo.MapLayout[character.Position.ToIndex()];
		DungeonInfo.Map[nextMove.ToIndex()] = character.Symbol;
		character.Position.x = nextMove.x;
		character.Position.y = nextMove.y;
		return 1;
	}

	if (flag == 2)
	{
		DungeonInfo.Hardness[nextMove.ToIndex()] -= 85;
		return 2;
	}
	return 0;
}

int Turn::KillCharacter(Point& position)
{
	if (DungeonInfo.Player.Position == position)
		return -1;

	int i;

	for (i = 0; i < DungeonInfo.NumMonster; i++)
	{
		if (position == DungeonInfo.Monsters[i].Position)
		{
			DungeonInfo.Monsters[i].Alive = 0;
			return 0;
		}
	}

	return -2;
}

int Turn::CheckLineOfSight(Point &position)
{
	int count = 0;
	Point current;
	current.x = position.x;
	current.y = position.y;

	int yMin = INT_MAX;
	int yMax = INT_MAX;


	while (DungeonInfo.Hardness[current.ToIndex()] == 0)
	{
		current.x--;

		if (DungeonInfo.Map[current.ToIndex()] == '@')
			return 1;

		while (DungeonInfo.Hardness[current.ToIndex()] == 0 && count <= yMin)
		{
			current.y--;
			if (DungeonInfo.Map[current.ToIndex()] == '@')
				return 1;

			count++;
		}
		current.y = position.y;
		yMin = count;
		count = 0;

		while (DungeonInfo.Hardness[current.ToIndex()] == 0 && count <= yMax)
		{
			current.y++;
			if (DungeonInfo.Map[current.ToIndex()] == '@')
				return 1;

			count++;
		}

		if (DungeonInfo.Map[current.ToIndex()] == '@')
			return 1;

		current.y -= count;
		yMax = count;
	}

	current.x = position.x;
	current.y = position.y;

	while (DungeonInfo.Hardness[current.ToIndex()] == 0)
	{

		while (DungeonInfo.Hardness[current.ToIndex()] == 0 && count <= yMin)
		{
			current.y--;
			if (DungeonInfo.Map[current.ToIndex()] == '@')
				return 1;

			count++;
		}
		current.y = position.y;
		yMin = count;
		count = 0;

		while (DungeonInfo.Hardness[current.ToIndex()] == 0 && count <= yMax)
		{
			current.y++;
			if (DungeonInfo.Map[current.ToIndex()] == '@')
				return 1;

			count++;
		}

		if (DungeonInfo.Map[current.ToIndex()] == '@')
			return 1;

		current.y -= count;
		yMax = count;

		current.x++;
		if (DungeonInfo.Map[current.ToIndex()] == '@')
			return 1;
	}

	return 0;
}

Point Turn::RandomMove(Point &from)
{
	Point result = from;
	switch (rand() % 8)
	{
		case 0:
			result.x++;
			break;

		case 1:
			result.x--;
			break;

		case 2:
			result.y++;
			break;

		case 3:
			result.y--;
			break;

		case 4:
			result.x++;
			result.y++;
			break;

		case 5:
			result.x++;
			result.y--;
			break;

		case 6:
			result.x--;
			result.y++;
			break;

		case 7:
			result.x--;
			result.y--;
			break;
	}

	return result;
}

void Turn::DisplayMonsterInfo(WINDOW* window, int offset)
{
	static int currentIndex = 0;
	int i, counter, monster = 0, total = 0;

	for (i = 0; i < DungeonInfo.NumMonster; i++)
	{
		if (DungeonInfo.Monsters[i].Alive == 1)
		{
			total++;
			if (i >= currentIndex)
				monster++;
		}
	}

	switch (offset)
	{
	case 1:
		if (monster - 6 > 0)
			currentIndex++;
		break;
	case -1:
		if (currentIndex != 0)
			currentIndex--;
		break;
	}


	wmove(window, 1, 2);
	wprintw(window, "There is %d Monsters. Use Up/Down Arrow to Scroll", total);

	int xOffset, yOffset, skip = 0;
	char dir1[5], dir2[6];

	for (i = 0, counter = 0; i < DungeonInfo.NumMonster && counter < 12; i++)
	{
		if (!DungeonInfo.Monsters[i].Alive)
			continue;

		if (skip < currentIndex)
		{
			skip++;
			continue;
		}

		xOffset = DungeonInfo.Player.Position.x - DungeonInfo.Monsters[i].Position.x;
		yOffset = DungeonInfo.Player.Position.y - DungeonInfo.Monsters[i].Position.y;

		wmove(window, ++counter + 1, 2);

		if (xOffset >= 0)
		{
			strcpy(dir1, "W");
		}
		else
		{
			strcpy(dir1, "E");
		}

		if (yOffset >= 0)
		{
			strcpy(dir2, "N");
		}
		else
		{
			strcpy(dir2, "S");
		}

		wprintw(window, "Monster %s", DungeonInfo.Monsters[i].Template->Name.c_str());
		wmove(window, counter + 2, 2);
		wprintw(window, "	Location %s %d	%s %d", dir1, abs(xOffset), dir2, abs(yOffset));
		counter++;
	}

	wrefresh(window);
}