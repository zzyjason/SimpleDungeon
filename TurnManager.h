#ifndef TURN_MANAGER_H_

#include <ncurses.h>

Heap* CreateTurnManager(MapInfo *mapInfo);
int ComparePC(void* PC1, void*PC2);
int PCAction(MapInfo *mapInfo, PC *Player);
int comparePoint(Point m, Point n);
int KillPC(MapInfo *mapInfo, Point position);
int NextTurn(MapInfo *mapInfo, Heap* turnManager);
int CheckOfSight(MapInfo *mapInfo, Point position);
void printHeap(Heap *heap);
int UserAction(MapInfo *mapInfo, PC *Player);
int ValidateNextMove(MapInfo *mapInfo, Point nextMove, int tunnle);
int MovePC(MapInfo *mapInfo, PC *Player, Point nextMove);
Point randomMove(Point from);
void DisplayMonsterInfo(MapInfo *mapInfo, WINDOW *info, int offset);

#endif // !TURN_MANAGER_H_
