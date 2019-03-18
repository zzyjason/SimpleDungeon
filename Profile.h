#ifndef PROFILE_H_
#define PROFILE_H_

#include "Dungeon.h"

namespace Profile
{
	
	void CreatProfileDir();
	void GetProfileDir();
	void LoadProfile(Dungeon &dungeon);
	void SaveProfile(Dungeon &dungeon);
	void loadHallway(Dungeon &dungeon);
	int ParseMonsterInfo(std::vector<MonsterTemplate> &list);
	bool ParseDiceInfo(char* line, Dice &dice);
	int ParseItemInfo(std::vector<ItemTemplate> &list);
}


#endif