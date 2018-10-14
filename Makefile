CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -lm -ggdb -lncurses
OBJFILES = Heap.o DungeonGenerator.o ProfileManager.o TurnManager.o  Path.o main.o 
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
	rm -f $(OBJFILES)

Heap.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c Heap.c -o Heap.o

DungeonGenerator.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c DungeonGenerator.c -o DungeonGenerator.o

ProfileManager.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c ProfileManager.c -o ProfileManager.o

Path.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c Path.c -o Path.o

TurnManager.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c TurnManager.c -o TurnManager.o


main.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c main.c -o main.o


clean:
	rm -f $(OBJFILES) $(TARGET) *~

