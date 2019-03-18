#ifndef UTIL_H_
#define UTIL_H_

extern const int HEIGHT;
extern const int WIDTH;

class Point
{
	public:
		unsigned char x;
		unsigned char y;
		int ToIndex();
		Point();
		Point(int index);
		Point(unsigned char X, unsigned char Y);
		static int CoordinatesToIndex(int x, int y);
		bool operator==(const Point& Other);
};

class Dice
{
	public:
		int Base;
		int N;
		int Sizes;
		Dice();
		Dice(int base, int n, int sizes);
		int Roll();
		void Print();
};

typedef struct Size
{
	int dx;
	int dy;
}Size;


#endif