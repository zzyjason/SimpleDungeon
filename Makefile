CC = g++
CFLAGS = -Wall -Werror
LDFLAGS = -lm -ggdb -lncurses
OBJFILES = Util.o Heap.o Dungeon.o Profile.o Turn.o  Path.o main.o Character.o 
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
	rm -f $(OBJFILES)

Util.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c Util.cpp -o Util.o

Character.o: 
	$(CC) $(CFLAGS) $(LDFLAGS) -c Character.cpp -o Character.o

Heap.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c Heap.cpp -o Heap.o

Dungeon.o: 
	$(CC) $(CFLAGS) $(LDFLAGS) -c Dungeon.cpp -o Dungeon.o

Profile.o: 
	$(CC) $(CFLAGS) $(LDFLAGS) -c Profile.cpp -o Profile.o

Path.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c Path.cpp -o Path.o

Turn.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c Turn.cpp -o Turn.o


main.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c main.cpp -o main.o


clean:
	rm -f $(OBJFILES) $(TARGET) *~

