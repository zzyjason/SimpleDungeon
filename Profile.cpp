#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <string>

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <ncurses.h>

#include "Dungeon.h"
#include "Profile.h"

namespace Profile
{

	
	char *profileDir = NULL;
	char *profile = NULL;

	int ParseItemInfo(std::vector<ItemTemplate> &list)
	{
		if (profileDir == NULL)
			CreatProfileDir();

		char* fileName = (char*)malloc(strlen(profile) + 17);
		strcpy(fileName, profileDir);
		strcat(fileName, "object_desc.txt");

		std::ifstream file(fileName);

		if (!file.good())
			return -1;

		char* line = (char*)malloc(255);
		file.getline(line, 254);

		if (strcmp(line, "RLG327 OBJECT DESCRIPTION 1") != 0)
			return -2;

		bool building = false;
		int Compelet = 0;

		while (file.getline(line, 254))
		{
			if (strcmp(line, "BEGIN OBJECT") == 0)
			{
				if (building)
				{
					list.pop_back();
				}

				Compelet = 0;
				building = true;
				list.push_back(ItemTemplate());
			}

			if (!building)
				continue;

			if (strcmp(line, "DESC") == 0)
			{
				if (Compelet & 0x0001)
				{
					building = false;
					list.pop_back();
					continue;
				}
				file.getline(line, 254);
				while (strcmp(line, ".") != 0)
				{
					list.back().Description += line;
					if (strlen(line) > 77)
					{
						building = false;
						list.pop_back();
						break;
					}
					list.back().Description += "\n";
					file.getline(line, 254);
				}
				list.back().Description.back() = 0;

				Compelet |= 0x1;
			}

			if (strncmp(line, "TYPE", 4) == 0)
			{
				if (Compelet & 0x0002)
				{
					building = false;
					list.pop_back();
					continue;
				}

				for (int i = 5; (size_t)i < strlen(line); i++)
				{
					list.back().Type += line[i];
				}
				bool match = false;
				for (int i = 0; i < ItemInfo::NumsTypeOfItem; i++)
				{
					if (list.back().Type.compare(ItemInfo::ItemType[i]) == 0)
					{
						match = true;
						break;
					}
				}

				if (match)
				{
					Compelet |= 0x2;
				}
				else
				{
					building = false;
					list.pop_back();
				}
				continue;
			}

			if (strncmp(line, "NAME", 4) == 0)
			{
				if (strlen(line) <= 5 || Compelet & 0x0004)
				{
					building = false;
					list.pop_back();
					continue;
				}
				list.back().Name = (line + 5);
				Compelet |= 0x4;
			}

			if (strncmp(line, "COLOR", 5) == 0)
			{
				if (strlen(line) <= 7 || Compelet & 0x0008)
				{
					building = false;
					list.pop_back();
					continue;
				}
				std::vector<std::string> colors;
				colors.push_back(std::string(""));
				for (int i = 6; (size_t)i < strlen(line); i++)
				{
					if (line[i] == ' ')
					{
						std::string color = colors.back();
						if (color.compare("RED") != 0 && color.compare("GREEN") != 0 && color.compare("BLUE") != 0 && color.compare("CYAN") != 0 && color.compare("YELLOW") != 0 && color.compare("MAGENTA") != 0 && color.compare("WHITE") != 0 && color.compare("BLACK") != 0)
						{
							colors.pop_back();
						}
						colors.push_back(std::string(""));

					}
					else
					{
						colors.back() += line[i];
					}

				}

				for (int i = 0; (size_t)i < colors.size(); i++)
				{
					if (colors[i].compare("RED") == 0)
						list.back().Color.push_back(COLOR_RED);
					else if (colors[i].compare("BLACK") == 0)
						list.back().Color.push_back(COLOR_BLACK);
					else if (colors[i].compare("GREEN") == 0)
						list.back().Color.push_back(COLOR_GREEN);
					else if (colors[i].compare("YELLOW") == 0)
						list.back().Color.push_back(COLOR_YELLOW);
					else if (colors[i].compare("BLUE") == 0)
						list.back().Color.push_back(COLOR_BLUE);
					else if (colors[i].compare("MAGENTA") == 0)
						list.back().Color.push_back(COLOR_MAGENTA);
					else if (colors[i].compare("CYAN") == 0)
						list.back().Color.push_back(COLOR_CYAN);
					else if (colors[i].compare("WHITE") == 0)
						list.back().Color.push_back(COLOR_WHITE);
				}


				Compelet |= 0x8;
			}

			if (strncmp(line, "HIT", 3) == 0)
			{
				if (strlen(line) < 9 || Compelet & 0x10)
				{
					building = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Hit))
				{
					building = false;
					list.pop_back();
					continue;
				}

				Compelet |= 0x10;
			}

			if (strncmp(line, "DAM", 3) == 0)
			{
				if (strlen(line) < 9 || Compelet & 0x20)
				{
					building = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Damage))
				{
					building = false;
					list.pop_back();
					continue;
				}
				Compelet |= 0x20;
			}

			if (strncmp(line, "DODGE", 5) == 0)
			{
				if (strlen(line) < 11 || Compelet & 0x40)
				{
					building = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Dodge))
				{
					building = false;
					list.pop_back();
					continue;
				}
				Compelet |= 0x40;
			}

			if (strncmp(line, "DEF", 3) == 0)
			{
				if (strlen(line) < 9 || Compelet & 0x80)
				{
					building = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Def))
				{
					building = false;
					list.pop_back();
					continue;
				}
				Compelet |= 0x80;
			}

			if (strncmp(line, "WEIGHT", 6) == 0)
			{
				if (strlen(line) < 11 || Compelet & 0x100)
				{
					building = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Weight))
				{
					building = false;
					list.pop_back();
					continue;
				}
				Compelet |= 0x100;
			}


			if (strncmp(line, "SPEED", 5) == 0)
			{
				if (strlen(line) < 11 || Compelet & 0x200)
				{
					building = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Speed))
				{
					building = false;
					list.pop_back();
					continue;
				}

				Compelet |= 0x200;
			}

			if (strncmp(line, "VAL", 3) == 0)
			{
				if (strlen(line) < 9 || Compelet & 0x400)
				{
					building = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Val))
				{
					building = false;
					list.pop_back();
					continue;
				}

				Compelet |= 0x400;
			}

			if (strncmp(line, "ART", 3) == 0)
			{
				if (strlen(line) < 8 || Compelet & 0x800)
				{
					building = false;
					list.pop_back();
					continue;
				}
				
				if (strcmp("TRUE", line + 4) == 0)
				{
					list.back().Art = true;
					Compelet |= 0x800;
				}
				else if (strcmp("FALSE", line + 4) == 0)
				{
					list.back().Art = false;
					Compelet |= 0x800;
				}
				else
				{
					building = false;
					list.pop_back();
				}
				continue;
			}

			if (strncmp(line, "RRTY", 4) == 0)
			{
				if (strlen(line) < 6 || Compelet & 0x01000)
				{
					building = false;
					list.pop_back();
					continue;
				}

				std::string val = "";
				for (int i = 5; (size_t)i < strlen(line); i++)
				{
					val += line[i];
				}

				try
				{
					list.back().Rarity = std::stoi(val);
				}
				catch (...) {
					building = false;
					list.pop_back();
					continue;
				}

				Compelet |= 0x01000;
			}

			if (strcmp(line, "END") == 0)
			{
				if (building)
				{
					if (Compelet != 0x01FFF)
						list.pop_back();

					building = false;
				}

			}
		}

		file.close();
		return 0;
	}

	int ParseMonsterInfo(std::vector<MonsterTemplate> &list)
	{
		if (profileDir == NULL)
			CreatProfileDir();

		char* fileName = (char*)malloc(strlen(profile) + 18);
		strcpy(fileName, profileDir);
		strcat(fileName, "/monster_desc.txt");

		std::ifstream file(fileName);

		if (!file.good())
			return -1;

		char* line = (char*)malloc(255);
		file.getline(line, 254);

		if (strcmp(line, "RLG327 MONSTER DESCRIPTION 1") != 0)
			return -2;


		
		bool buildingMonster = false;
		int MonsterCompelet = 0;
		


		while (file.getline(line, 254))
		{
			if (strcmp(line, "BEGIN MONSTER") == 0)
			{
				if (buildingMonster)
				{
					MonsterCompelet = 0;
					list.pop_back();
					list.push_back(MonsterTemplate());
				}
				else
				{
					MonsterCompelet = 0;
					list.push_back(MonsterTemplate());
					buildingMonster = true;
				}
			}

			if (!buildingMonster)
				continue;
			
			if (strncmp(line, "NAME", 4) == 0)
			{
				if (strlen(line) <= 5 || MonsterCompelet & 0x0001)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}
				list.back().Name = (line + 5);
				MonsterCompelet |= 0x1;
			}
			
			if (strncmp(line, "SYMB", 4) == 0)
			{
				if (strlen(line) !=6 || MonsterCompelet & 0x0002)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}
				list.back().Symbol = line[5];
				MonsterCompelet |= 0x2;
			}

			if (strcmp(line, "DESC") == 0)
			{
				if (MonsterCompelet & 0x0004)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}
				file.getline(line, 254);
				while (strcmp(line, ".") != 0)
				{
					list.back().Description += line;
					if (strlen(line) > 77)
					{
						buildingMonster = false;
						list.pop_back();
						break;
					}
					list.back().Description += "\n";
					file.getline(line, 254);
				}
				list.back().Description.back() = 0;

				MonsterCompelet |= 0x4;
			}

			if (strncmp(line, "COLOR", 5) == 0)
			{
				if (strlen(line) <= 7 || MonsterCompelet & 0x0008)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				std::vector<std::string> colors;
				colors.push_back(std::string(""));
				for (int i = 6; (size_t)i < strlen(line); i++)
				{
					if (line[i] == ' ')
					{
						std::string color = colors.back();
						if (color.compare("RED") != 0 && color.compare("GREEN") != 0 && color.compare("BLUE") != 0 && color.compare("CYAN") != 0 && color.compare("YELLOW") != 0 && color.compare("MAGENTA") != 0 && color.compare("WHITE") != 0 && color.compare("BLACK") != 0)
						{
							colors.pop_back();
						}
						colors.push_back(std::string(""));

					}
					else
					{
						colors.back() += line[i];
					}

				}

				for (int i = 0; (size_t)i < colors.size(); i++)
				{
					if (colors[i].compare("RED") == 0)
						list.back().Color.push_back(COLOR_RED);
					else if (colors[i].compare("BLACK") == 0)
						list.back().Color.push_back(COLOR_BLACK);
					else if (colors[i].compare("GREEN") == 0)
						list.back().Color.push_back(COLOR_GREEN);
					else if (colors[i].compare("YELLOW") == 0)
						list.back().Color.push_back(COLOR_YELLOW);
					else if (colors[i].compare("BLUE") == 0)
						list.back().Color.push_back(COLOR_BLUE);
					else if (colors[i].compare("MAGENTA") == 0)
						list.back().Color.push_back(COLOR_MAGENTA);
					else if (colors[i].compare("CYAN") == 0)
						list.back().Color.push_back(COLOR_CYAN);
					else if (colors[i].compare("WHITE") == 0)
						list.back().Color.push_back(COLOR_WHITE);
				}

				MonsterCompelet |= 0x8;
			}
			
			if (strncmp(line, "SPEED", 5) == 0)
			{
				if (strlen(line) < 11 || MonsterCompelet & 0x10)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}
				
				if (!Profile::ParseDiceInfo(line, list.back().Speed))
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				MonsterCompelet |= 0x10;
			}

			if (strncmp(line, "DAM", 3) == 0)
			{
				if (strlen(line) < 9 || MonsterCompelet & 0x20)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().Damage))
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				MonsterCompelet |= 0x20;
			}

			if (strncmp(line, "HP", 2) == 0)
			{
				if (strlen(line) < 8 || MonsterCompelet & 0x40)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				if (!Profile::ParseDiceInfo(line, list.back().HP))
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				MonsterCompelet |= 0x40;
			}

			if (strncmp(line, "ABIL", 4) == 0)
			{
				if (MonsterCompelet & 0x80)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				std::string ability = "";
				list.back().Ability = 0;
				for (int i = 5; (size_t)i < strlen(line); i++)
				{
					if (line[i] == ' ')
					{
						if (ability.compare("SMART") == 0)
						{
							list.back().Ability |= 0b01000;
						}
						else if (ability.compare("TELE") == 0)
						{
							list.back().Ability |= 0b00100;
						}
						else if (ability.compare("TUNNEL") == 0)
						{
							list.back().Ability |= 0b00010;
						}
						else if (ability.compare("ERRATIC") == 0)
						{
							list.back().Ability |= 0b00001;
						}
						else if (ability.compare("PASS") == 0)
						{
							list.back().Ability |= 0b010000;
						}
						else if (ability.compare("PICKUP") == 0)
						{
							list.back().Ability |= 0b0100000;
						}
						else if (ability.compare("DESTROY") == 0)
						{
							list.back().Ability |= 0b01000000;
						}
						else if (ability.compare("UNIQ") == 0)
						{
							list.back().Ability |= 0b010000000;
						}
						else if (ability.compare("BOSS") == 0)
						{
							list.back().Ability |= 0b0100000000;
						}

						ability = "";
					}
					else
					{
						ability += line[i];
					}
				}

				if (ability.compare("SMART") == 0)
				{
					list.back().Ability |= 0b01000;
				}
				else if (ability.compare("TELE") == 0)
				{
					list.back().Ability |= 0b00100;
				}
				else if (ability.compare("TUNNEL") == 0)
				{
					list.back().Ability |= 0b00010;
				}
				else if (ability.compare("ERRATIC") == 0)
				{
					list.back().Ability |= 0b00001;
				}
				else if (ability.compare("PASS") == 0)
				{
					list.back().Ability |= 0b010000;
				}
				else if (ability.compare("PICKUP") == 0)
				{
					list.back().Ability |= 0b0100000;
				}
				else if (ability.compare("DESTROY") == 0)
				{
					list.back().Ability |= 0b01000000;
				}
				else if (ability.compare("UNIQ") == 0)
				{
					list.back().Ability |= 0b010000000;
				}
				else if (ability.compare("BOSS") == 0)
				{
					list.back().Ability |= 0b0100000000;
				}

				MonsterCompelet |= 0x80;
			}

			if (strncmp(line, "RRTY", 4) == 0)
			{
				if (strlen(line) < 6 || MonsterCompelet & 0x0100)
				{
					buildingMonster = false;
					list.pop_back();
					continue;
				}

				std::string val = "";
				for (int i = 5; (size_t)i < strlen(line); i++)
				{
					val += line[i];
				}
				
				try
				{
					list.back().Rarity = std::stoi(val);
				}
				catch(...){
					buildingMonster = false;
					list.pop_back();
					continue;
				}
				
				MonsterCompelet |= 0x0100;
			}

			if (strcmp(line, "END") == 0)
			{
				if (MonsterCompelet)
				{
					if (MonsterCompelet != 511)
						list.pop_back();

					buildingMonster = false;
				}

			}
		}

		file.close();

		return 0;
	}

	bool ParseDiceInfo(char* line, Dice &dice)
	{
		std::string val;
		int i;
		bool Base = false;
		bool N = false;
		bool Sizes = false;
		for (i = 0; (size_t)i < strlen(line); i++)
		{
			if (line[i] == '+')
			{
				try
				{
					dice.Base = std::stoi(val);
				}
				catch (...) {
					return false;
				}
				val = "";
				Base = true;
				continue;
			}
			else if (line[i] == 'd')
			{
				try
				{
					dice.N = std::stoi(val);
				}
				catch (...) {
					return false;
				}
				val = "";
				N = true;
				continue;
			}
			else if (line[i] == ' ')
			{
				val = "";
				continue;
			}

			val += line[i];
		}
		if (val.length() == 0)
		{
			return false;
		}

		try
		{
			dice.Sizes = std::stoi(val);
			Sizes = true;
			
		}
		catch (...) {
			return false;
		}

		if (dice.N < 0 || dice.Sizes < 0 || !Base || !N || !Sizes) 
		{
			return false;
		}

		return true;
	}


	void CreatProfileDir()
	{
		GetProfileDir();
		struct stat b;
		if (stat(profileDir, &b) == -1)
			mkdir(profileDir, 0770);
	}
	void GetProfileDir()
	{
		char *homedir = getenv("HOME");
		size_t homedirlen = strlen(homedir);
		char *result = (char*)malloc(sizeof(char) * (homedirlen + 10));
		profile = (char*)malloc(sizeof(char) * (homedirlen + 20));

		strcpy(result, homedir);
		strcat(result, "/.rlg327/");

		strcpy(profile, result);
		strcat(profile, "dungeon");
		profileDir = result;
	}

	void LoadProfile(Dungeon &dungeon)
	{
		if (profileDir == NULL)
			CreatProfileDir();

		FILE *file = fopen(profile, "r");
		if (file == NULL)
		{
			return;
		}

		char *Title = (char*)malloc(12);
		fread(Title, 1, 12, file);

		int fileVersion = 0;
		fread(&fileVersion, sizeof(int), 1, file);
		fileVersion = be32toh(fileVersion);

		int fileSize = 0;
		fread(&fileSize, sizeof(int), 1, file);
		fileSize = be32toh(fileSize);

		fread(&(dungeon.Player.Position.x), 1, 1, file);
		fread(&(dungeon.Player.Position.y), 1, 1, file);
		fread(&dungeon.Hardness, 1, 80 * 21, file);

		int BytesToRead = fileSize - ftell(file);
		int i = 0;
		dungeon.NumsRoom = BytesToRead / 4;

		dungeon.GenerateRooms();

		for (i = 0; i < dungeon.NumsRoom; i++)
		{
			fread(&dungeon.Rooms[i].TopLeft.x, 1, 1, file);
			fread(&dungeon.Rooms[i].TopLeft.y, 1, 1, file);
			fread(&dungeon.Rooms[i].Sizes.dx, 1, 1, file);
			fread(&dungeon.Rooms[i].Sizes.dy, 1, 1, file);
			dungeon.PlaceRoomWorker(dungeon.Rooms[i]);
		}

		fclose(file);
		Profile::loadHallway(dungeon);
	}
	void SaveProfile(Dungeon &dungeon)
	{
		if (profileDir == NULL)
			CreatProfileDir();

		FILE *file = fopen(profile, "w");
		int fileVersion = htobe32(555);

		int fileSize = 1702 + ((int)dungeon.NumsRoom) * 4;
		fileSize = htobe32(fileSize);


		fprintf(file, "RLG327-F2018");
		fwrite(&fileVersion, sizeof(int), 1, file);
		fwrite(&fileSize, sizeof(int), 1, file);
		fwrite(&dungeon.Player.Position.x, 1, 1, file);
		fwrite(&dungeon.Player.Position.y, 1, 1, file);

		fwrite(&dungeon.Hardness, 1, 80 * 21, file);

		int i = 0;
		for (; i < dungeon.NumsRoom; i++)
		{
			fwrite(&dungeon.Rooms[i].TopLeft.x, 1, 1, file);
			fwrite(&dungeon.Rooms[i].TopLeft.y, 1, 1, file);
			fwrite(&dungeon.Rooms[i].Sizes.dx, 1, 1, file);
			fwrite(&dungeon.Rooms[i].Sizes.dy, 1, 1, file);
		}

		fclose(file);
	}
	void loadHallway(Dungeon &dungeon)
	{
		int i;
		for (i = 0; i < 80 * 21; i++)
		{
			if (dungeon.Map[i] == ' ' && dungeon.Hardness[i] == 0)
			{
				dungeon.Map[i] = '#';
			}
		}
	}
}