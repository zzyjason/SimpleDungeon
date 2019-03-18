#ifndef CHARACTER_H_
#define CHARACTER_H_

#include "Util.h"
#include <string>
#include <vector>

class MonsterTemplate
{
public:
	std::string Name;
	std::string Description;

	std::vector<short> Color;
	
	unsigned int Ability;
	int Rarity;
	char Symbol;
	bool valid;

	Dice Damage;
	Dice Speed;
	Dice HP;
	MonsterTemplate();
	void Print();
};

class ItemTemplate
{
	public:
		std::string Name;
		std::string Description;
		std::vector<short> Color;

		std::string Type;


		Dice Hit;
		Dice Damage;
		Dice Dodge;
		Dice Def;
		Dice Weight;
		Dice SpeedAtt;
		Dice Speed;
		Dice Attribute;
		Dice Val;
		bool Art;
		bool valid = true;

		int Rarity;
		void Print();
};

class Item
{
	public:

		ItemTemplate* Template;
		short Color;
		char Symbol;
		int Hit = 0;
		int Damage = 0;
		int Dodge = 0;
		int Def = 0;
		int Weight = 0;
		int SpeedAtt = 0;
		int Speed = 0;
		int Attribute = 0;
		int Val = 0;
		Point Position;
		Item();
		Item(ItemTemplate* itemTemplate);
};

class Character
{
	public:
		Point Position;
		bool Alive = true;
		char Symbol;
		bool User;

		int GetDamage();
		int Speed;
		int HP;
		int Damage;

		int Dodge = 0;
		int Defense = 0;
		int Weight = 0;
		

		int NextRound = 0;

		Character();
		void UpdateNextRound(int currentRound);

};

class NPC : public Character
{
	public:
		Point LastKnownPC;
		void operator=(const NPC& b);
		short Color;
		unsigned int Ability;
		int Rarity;
		MonsterTemplate* Template;
		

		NPC();
		NPC(MonsterTemplate* monster);
};

class PC :public Character
{
	public:
		PC();
		Item* Bag[10];
		Item* Weapon[2];
		Item* Armor = NULL;
		Item* Ranged = NULL;
		Item* Helmet = NULL;
		Item* Cloak = NULL;
		Item* Gloves = NULL;
		Item* Boots = NULL;
		Item* Amulet = NULL;
		Item* Light = NULL;
		Item* Rings[2];
		int MaxHP;
		int GetDamage();
		
};

namespace ItemInfo
{
	extern int NumsTypeOfItem;
	extern char ItemType[19][20];
	extern char ItemSymbol[19];
	
}

#endif