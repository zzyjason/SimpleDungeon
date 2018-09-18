all: project

project: main.c
	gcc -o main main.c DungeonGenerator.c ProfileManager.c Path.c -lm

clean:
	rm -f main

