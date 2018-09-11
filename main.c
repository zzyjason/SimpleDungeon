#include "DungeonGenerator.h"
#include <stdlib.h>

int main()
{
    MapInfo info = GenerateNewMap();
	free(info.map);
	free(info.rooms);
    return 0;
}
