#ifndef PATH_H_

void UpdatePath(MapInfo *mapInfo);
void PathWalker(MapInfo *mapInfo, Point Des, Point Src, int Tunnel);
int PathChecker(MapInfo *mapInfo, Point position, int Tunnel);
void printPath(MapInfo *mapInfo);

#endif // !PATH_H_
