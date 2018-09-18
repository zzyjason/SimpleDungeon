#ifndef PROFILE_MANAGER_H_
#define PROFILE_MANAGER_H_

#include "main.h"

void CreatProfileDir();
void GetProfileDir();
void LoadProfile(MapInfo *mapInfo);
void SaveProfile(MapInfo *mapInfo);
void loadHallway(MapInfo *mapInfo);

#endif