all: project

project0: main.c
	gcc -Wall -Werror -g main.c -o main -lm

clean:
	rm -f main

