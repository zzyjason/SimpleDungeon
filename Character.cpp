#include "Character.h"
#include "Profile.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

Character::Character()
{

}

void Character::UpdateNextRound(int currentRound)
{
	NextRound = currentRound + 1000 / Speed;
}

int Character::GetDamage()
{
	return Damage;
}

int PC::GetDamage()
{
	int result = 0;
	if (Weapon[0] != NULL)
	{
		result += Weapon[0]->Damage;
	}
	if (Weapon[1] != NULL)
	{
		result += Weapon[1]->Damage;
	}
	if (Damage > result)
		return Damage;
	return result;
}

PC::PC()
{
	User = true;
	Symbol = '@';
	Speed = 10;
	HP = MaxHP = 10000;
	for (int i = 0; i < 10; i++)
		Bag[i] = NULL;
	for (int i = 0; i < 2; i++)
	{
		Weapon[i] = NULL;
		Rings[i] = NULL;
	}
	Dodge = 0;
	Defense = 0;
	Weight = 0;
	Damage = 50;
}

NPC::NPC()
{
	User = false;
	Dodge = 0;
	Defense = 0;
	Weight = 0;
}

void NPC::operator=(const NPC& b)
{
	Template = b.Template;
	Symbol = b.Symbol;
	Speed = b.Speed;
	HP = b.HP;
	Damage = b.Damage;
	Ability = b.Ability;
	Rarity = b.Rarity;
	NextRound = b.NextRound;
	User = b.User;
	Alive = b.Alive;
	Position = b.Position;
	Color = b.Color;
}


NPC::NPC(MonsterTemplate* monster)
{
	Template = monster;
	Symbol = monster->Symbol;
	Speed = monster->Speed.Roll();
	HP = monster->HP.Roll();
	Damage = monster->HP.Roll();
	Ability = monster->Ability;
	Rarity = monster->Rarity;
	Color = monster->Color[0];
	Dodge = 0;
	Defense = 0;
	Weight = 0;
}

MonsterTemplate::MonsterTemplate()
{
	Name = "";
	Description = "";
	Ability = 0;
	Rarity = 0;
	valid = true;
}

void MonsterTemplate::Print()
{
	cout << "BEGIN MONSTER" << endl;

	cout << "NAME " << Name << endl;
	cout << "DESC" << endl;
	cout << Description << endl << "." << endl;
	cout << "COLOR";
	for (int i = 0; (size_t)i < Color.size(); i++)
	{
		cout << " " << Color[i];
	}

	cout << endl;
	
	cout << "SPEED ";
	Speed.Print();

	cout << "ABIL";
	if (Ability & 0b01000)
	{
		cout << " SMART";
	}
	if (Ability & 0b00100)
	{
		cout << " TELE";
	}
	if (Ability & 0b00010)
	{
		cout << " TUNNEL";
	}
	if (Ability & 0b00001)
	{
		cout << " ERRATIC";
	}
	if (Ability & 0b010000)
	{
		cout << " PASS";
	}
	if (Ability & 0b0100000)
	{
		cout << " PICKUP";
	}
	if (Ability & 0b01000000)
	{
		cout << " DESTORY";
	}
	if (Ability & 0b010000000)
	{
		cout << " UNIQ";
	}
	if (Ability & 0b0100000000)
	{
		cout << " BOSS";
	}
	cout << endl;

	cout << "HP ";
	HP.Print();

	cout << "DAM ";
	Damage.Print();

	cout << "SYMB " << Symbol << endl;
	cout << "RRTY " << to_string(Rarity) << endl;


	cout << "END" << endl;
}

void ItemTemplate::Print()
{
	cout << "BEGIN OBJECT" << endl;
	cout << "NAME " << Name << endl;
	cout << "DESC" << endl;
	cout << Description << endl << "." << endl;
	cout << "TYPE " << Type << endl;

	cout << "COLOR";
	for (int i = 0; (size_t)i < Color.size(); i++)
	{
		cout << " " << Color[i];
	}
	cout << endl;

	cout << "HIT ";
	Hit.Print();
	cout << "DAM ";
	Damage.Print();
	cout << "DODGE ";
	Dodge.Print();
	cout << "DEF ";
	Def.Print();
	cout << "WEIGHT ";
	Weight.Print();
	cout << "SPEED ";
	Speed.Print();
	cout << "ATTR ";
	Attribute.Print();
	cout << "VAL ";
	Val.Print();
	cout << "ART ";
	if (Art)
		cout << "TRUE" << endl;
	else
		cout << "FALSE" << endl;

	cout << "RRTY " << to_string(Rarity) << endl;
	cout << "END" << endl;
}


Item::Item()
{

}

Item::Item(ItemTemplate* itemTemplate)
{
	Template = itemTemplate;
	Hit = itemTemplate->Hit.Roll();
	Damage = itemTemplate->Damage.Roll();
	Dodge = itemTemplate->Dodge.Roll();
	Def = itemTemplate->Def.Roll();
	Weight = itemTemplate->Weight.Roll();
	SpeedAtt = itemTemplate->SpeedAtt.Roll();
	Speed = itemTemplate->Speed.Roll();
	Attribute = itemTemplate->Attribute.Roll();
	Val = itemTemplate->Val.Roll();
	Color = itemTemplate->Color[0];
	
	Symbol = '*';
	for (int i = 0; i < ItemInfo::NumsTypeOfItem; i++)
	{
		if (itemTemplate->Type.compare(ItemInfo::ItemType[i]) == 0)
		{
			Symbol = ItemInfo::ItemSymbol[i];
			break;
		}
	}
}

namespace ItemInfo
{
	int NumsTypeOfItem = 19;
	char ItemType[19][20] = { "WEAPON", "OFFHAND", "RANGED", "ARMOR", "HELMET", "CLOAK", "GLOVES", "BOOTS", "RING", "AMULET", "LIGHT", "SCROLL", "BOOK", "FLASK", "GOLD", "AMMUNITION", "FOOD", "WAND", "CONTAINER" };
	char ItemSymbol[19] = { '|', ')', '}' , '[' , ']' , '(' , '{' , '\\' , '=' , '"' , '_' , '~' , '?' , '!' , '$' , '/' , ',' , '-' , '%' };
}