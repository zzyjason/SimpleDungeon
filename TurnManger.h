#ifndef TURN_MANAGER_H_

Heap* CreateTurnManager(MapInfo *mapInfo);
int ComparePC(void* PC1, void*PC2);
int PCAction(MapInfo *mapInfo, PC *Player);
int comparePoint(Point m, Point n);
int KillPC(MapInfo *mapInfo, Point position);
unsigned int NextTurn(MapInfo *mapInfo, Heap* turnManager);
int CheckOfSight(MapInfo *mapInfo, Point position);

#endif // !TURN_MANAGER_H_
