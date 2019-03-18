#include "Util.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

extern const int HEIGHT = 21;
extern const int WIDTH = 80;

int Point::ToIndex()
{
	return WIDTH * y + x;
}

Point::Point(int index)
{
	x = index % WIDTH;
	y = index / WIDTH;
}

int Point::CoordinatesToIndex(int x, int y)
{
	return WIDTH * y + x;
}

Point::Point()
{
	x = 0;
	y = 0;
}

Point::Point(unsigned char X, unsigned char Y)
{
	x = X;
	y = Y;
}

bool Point::operator==(const Point& Other)
{
	if (this->x == Other.x && this->y == Other.y)
		return true;
	return false;
}

Dice::Dice(int base, int n, int sizes)
{
	Base = base;
	N = n;
	Sizes = sizes;
}

int Dice::Roll()
{
	int result = Base;

	int i;
	for (i = 0; i < N; i++)
	{
		result += rand() % Sizes + 1;
	}

	return result;
}

Dice::Dice()
{

}

void Dice::Print()
{
	cout << Base << "+" << N << "d" << Sizes << endl;
}

