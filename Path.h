#ifndef PATH_H_
#define PATH_H_

#include "Util.h"
#include "Dungeon.h"



class Dungeon;

class Path
{
	private:
		void PathWalker(Dungeon &dungeon, Point des, Point src, bool tunnel);
		bool PathChecker(Dungeon &dungeon, Point position, bool tunnel);


	public:
		unsigned short Distance[80 * 21];
		Point ShortestPath[80 * 21];
		Path();
		Path(bool tunnle, Dungeon dungeon);
		void Construct(bool tunnle, Dungeon dungeon);
};

#endif