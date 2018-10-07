CC = gcc
CFLAGS = -WALL -Werror
LDFLAGS = -lm -ggdb
OBJFILES = Heap.o DungeonGenerator.o ProfileManager.o TurnManager.o  main.o
TARGET = main


$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

Heap.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c Heap.c -o Heap.o

DungeonGenerator.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c DungeonGenerator.c -o DungeonGenerator.o

ProfileManager.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c ProfileManager.c -o ProfileManager.o

TurnManager.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c TurnManager.c -o TurnManager.o

main.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c main.c -o main.o

clean:
	rm -f $(OBJFILES) $(TARGET) *~

