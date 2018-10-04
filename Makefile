all: project

project: main.c
	gcc -Wall -Werror -ggdb -o main main.c DungeonGenerator.c ProfileManager.c Path.c Heap.c TurnManger.c -lm

clean:
	rm -f main