all: project

project: main.c
	gcc -o main main.c DungeonGenerator.c -lm

clean:
	rm -f main

