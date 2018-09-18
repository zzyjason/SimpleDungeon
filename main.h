#ifndef MAIN_H_
#define MAIN_H_

typedef struct
{
	unsigned char x;
	unsigned char y;
}Point;

typedef struct
{
	Point Position;
	unsigned char Hardness;
}Block;

typedef struct
{
	unsigned char dx;
	unsigned char dy;

}Size;

typedef struct
{
	Point topLeft;
	Size size;
}Room;

typedef struct
{
	unsigned short Distance[80 * 21];
	Point ShortestPath[80 * 21];
}Path;

typedef struct
{
	Room *rooms;
	unsigned char numsRoom;
	Size size;
	char *map;
	unsigned char hardness[80 * 21];
	Point playerPosition;
	Path nonTunnelPath;
	Path TunnelPath;

}MapInfo;

#endif // !
