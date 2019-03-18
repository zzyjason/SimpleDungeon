#ifndef TURN_H_
#define TURN_H_

#include "Heap.h"
#include "Dungeon.h"
#include "Character.h"
#include <ncurses.h>

class Turn
{
	public:

		Turn(int maxChar, Dungeon &dungeon, int* level);
		int NextTurn();
		static int ComparePC(const Character* a, const Character* b);
		Heap Queue;

	private:
		Dungeon DungeonInfo;
		int* Level;

		Point SelectSpot(bool fog);
		void DisplayItemInfo(WINDOW* window, Item* item);
		void DisplayMonsterWindowInfo(WINDOW* window, NPC* monster);
		void EquipItem(Item* item, bool DeEquip);

		int SelectItem();
		void DisplayItems(WINDOW *window);
		void DisplayEquipment(WINDOW *window);
		void EquipmentDisplayWorker(WINDOW* window, Item* cur);
		void DisplayUI(bool fog);
		bool validateTP(int x, int y);
		int NPCAction(NPC &character);
		int KillCharacter(Point& position);
		int CheckLineOfSight(Point &position);
		int UserAction();
		int ValidateNextMove(Point &nextMove, bool tunnle);
		int MovePC(Point &nextMove, Character &character);
		Point RandomMove(Point &from);
		void DisplayMonsterInfo(WINDOW *window, int offset);

};

#endif