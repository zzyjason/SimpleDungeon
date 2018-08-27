#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

char* GenerateNewMap();
void placeRoom(char *map,int *roomLocation,int room, int plan, int dx, int dy);
int CheckEnoughRoom(char *map, int startX, int startY,int dx, int dy);
int placePlanChecker(char *map, int x, int y, int dx, int dy, int plan, int minX, int minY);
void placeHallway(char *map,int x1, int y1, int x2, int y2);
int count = 0;

char* GenerateNewMap()
{
    int i,j,counter, x, y;
    srand(time(NULL));
    char *NewMap = (char*) malloc(80 * 24 * sizeof(char));
    for(i=0; i< 80*64; i++)
    {
        NewMap[i] = ' ';
    }

    int numsRoom = rand()%3 + 6;
    int *roomPlan = (int*) malloc(3 * numsRoom * sizeof(int));
    int *roomLocation = (int*) malloc(2 * numsRoom * sizeof(int));
    for(i=0; i< 2*numsRoom; i++)
    {
        roomLocation[i] = -1;
    }

    int totalRoomArea = (rand()%10 + 20) * 80 * 24 / 100;
    int totalHallArea = (rand()%10 + 15) * 80 * 24 / 100;
    int AvgArea = totalRoomArea / numsRoom;
    printf("numsRoom: %d\n", numsRoom);
    printf("Total Room Area: %d\nTotal Hall Area: %d\nAvgArea: %d\n", totalRoomArea, totalHallArea, AvgArea);

    for(i=0, counter = 0; i < numsRoom; i++)
    {
        roomPlan[counter++] = rand() % 11;
        roomPlan[counter++] = (int) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
        roomPlan[counter++] = (int) sqrt((double) ((rand() % 13) * AvgArea / 10)) + 3;
        printf("\nPlan: %d    dx: %d    dy:%d\n", roomPlan[counter - 3], roomPlan[counter - 2], roomPlan[counter - 1]);
        placeRoom(NewMap, roomLocation, i, roomPlan[counter - 3], roomPlan[counter - 2], roomPlan[counter - 1]);
    }

    int *hallwayPoint = (int*) malloc (numsRoom * sizeof(int));
    for(i=0; i<numsRoom/3 + 1; i++)
    {
        do{
            x = rand()%78 + 1;
            y = rand()%22 + 1;
        } while(NewMap[y*80+x]!=' ');

        hallwayPoint[i*2] = x;
        hallwayPoint[i*2 + 1] = y;
        NewMap[y*80+x] = '#';
    }
    count = 0;
    for(i=0; i<numsRoom; i++)
    {
        int randomSkip = rand()%100;

        x = roomLocation[i*2];
        y = roomLocation[i*2+1];
        int dx = roomPlan[i*3+1];
        int dy = roomPlan[i*3+2];
        int startX = roomLocation[i*2];
        int startY = roomLocation[i*2+1];
        printf("Room: %d    StartX: %d    StartY: %d\n", i, startX, startY);

        while(NewMap[y*80+x]==' ')
        {
            x++;
            if(x>startX+dx)
            {
                x = startX;
                y++;
            }
        }
        int dir = 0;
        while(randomSkip!=0)
        {
            switch(dir)
            {
                case 0:
                    if(y-1>=startY && NewMap[(y-1)*80 + x] == '.')
                    {
                        dir = 3;
                        break;
                    }
                    else if(x+1>startX+dx || NewMap[y*80 + x + 1] != '.')
                    {
                        dir = 1;
                        break;
                    }
                    x++;
                    break;
                case 1:
                    if(x+1<=startX+dx && NewMap[y*80 + x + 1] == '.')
                    {
                        dir = 0;
                        break;
                    }
                    else if(y+1>startY+dy || NewMap[(y+1)*80 + x] != '.')
                    {
                        dir = 2;
                        break;
                    }
                    y++;
                    break;
                case 2:
                    if(y+1<=startY+dy && NewMap[(y+1)*80 + x] == '.')
                    {
                        dir = 1;
                        break;
                    }
                    else if(x-1<startX || NewMap[y*80 + x - 1] != '.')
                    {
                        dir = 3;
                        break;
                    }
                    x--;
                    break;
                case 3:
                    if(x-1>=startX && NewMap[y*80 + x -1] == '.')
                    {
                        dir = 2;
                        break;
                    }
                    else if(y-1<startY || NewMap[(y-1)*80 + x] != '.')
                    {
                        dir = 0;
                        break;
                    }
                    y--;
                    break;
            }
            randomSkip--;
        }

        int nextHallwayPoint = i%(numsRoom/3 + 1);
        placeHallway(NewMap, x, y, hallwayPoint[nextHallwayPoint * 2], hallwayPoint[nextHallwayPoint* 2 + 1]);
        nextHallwayPoint = (i+1)%(numsRoom/3 + 1);
        placeHallway(NewMap, x, y, hallwayPoint[nextHallwayPoint * 2], hallwayPoint[nextHallwayPoint* 2 + 1]);
    }

    printf("\n----------------------------------------------------------------------------------\n");
    for(i=0; i<24; i++)
    {
        printf("|");
        for(j=0; j<80; j++)
        {
            printf("%c", NewMap[i*80 + j]);
        }
        printf("|\n");
    }
    printf("----------------------------------------------------------------------------------\n");
}

void placeHallway(char *map,int x1, int y1, int x2, int y2)
{
    int current_x = x1;
    int current_y = y1;
    while(current_x != x2 || current_y != y2)
    {

        if(current_x - x2 > 0)
        {
            current_x--;
        }
        else if(current_x - x2 < 0)
        {
            current_x++;
        }

        if(map[current_y*80 + current_x] == ' ')
        {
            map[current_y*80 + current_x] = '#';
        }

        if(current_y - y2 > 0)
        {
            current_y--;
        }
        else if(current_y - y2 < 0)
        {
            current_y++;
        }

        if(map[current_y*80 + current_x] == ' ')
        {
            map[current_y*80 + current_x] = '#';
        }
    }
}

void placeRoom(char *map,int *roomLocation, int room, int plan, int dx, int dy)
{
    int startX = rand()%74 + 1;
    int startY = rand()%19 +1;
    int counter = 0;
    int i,j;

    while(1)
    {
        counter ++;
        if(CheckEnoughRoom(map, startX, startY, dx, dy) == 1)
        {
            roomLocation[room*2] = startX;
            roomLocation[room*2+1] = startY;

            int minX = rand()%(dx/2);
            int minY = rand()%(dy/2);
            if(minX<3)
            {
                minX = 3;
            }
            if(minY<3)
            {
                minY = 3;
            }

            for(i=0; i<dy; i++)
            {
                for (j = 0; j < dx; j++)
                {
                    if (placePlanChecker(map, j, i, dx, dy, plan, minX, minY))
                    {
                        map[(startY + i)*80 + startX + j] = '.';//count + '0';
                    }
                }
            }

            count++;
            break;
        }
        else
        {
            startX = rand()%75 + 1;
            startY = rand()%20 +1;
        }

        if(counter > 100)
        {
            printf("Failed Placed Room");
            return;
        }

    }

    printf("Successfully Placed Room, Counter: %d", counter);
}

int CheckEnoughRoom(char *map, int startX, int startY,int dx, int dy)
{
    int i, j, current;
    for(i=0; i<dy; i++)
    {
        for(j=0; j<dx; j++)
        {
            current = (startY + i)*80 + startX + j;
            if(startY+i >= 23 || startX+j >= 79 || startY+i <= 0 || startX+j <= 0 || map[current] != ' ' || map[current -1] != ' ' || map[current + 1] != ' ' || map[current + 80] != ' ' || map[current-80] != ' ' )
            {
                return 0;
            }
        }
    }
    return 1;
}

int placePlanChecker(char *map, int x, int y, int dx, int dy, int plan, int minX, int minY)
{
    if(plan == 10)
    {
        return 1;
    }
    int extraSpaceX = (dx-minX) / 2;
    int extraSpaceY = (dy-minY) / 2;
    switch(plan)
    {
        case 0:
            if((y>=extraSpaceY && y<=extraSpaceY + minY) || (x>=extraSpaceX && x<=extraSpaceX + minX))
            {
                return 1;
            }
            break;
        case 1:
            if(y<=minY || (x>=extraSpaceX && x<=extraSpaceX + minX))
            {
                return 1;
            }
            break;
        case 2:
            if((y>=extraSpaceY && y<=extraSpaceY + minY) || (x>=extraSpaceX*2))
            {
                return 1;
            }
            break;
        case 3:
            if((y>=extraSpaceY && y<=extraSpaceY + minY) || (x<=minX))
            {
                return 1;
            }
            break;
        case 4:
            if(y>=extraSpaceY*2 || (x>=extraSpaceX && x<=minX + extraSpaceX))
            {
                return 1;
            }
            break;
        case 5:
            if(y<=minY || x<=minX)
            {
                return 1;
            }
            break;
        case 6:
            if(y<=minY || x>=extraSpaceX*2)
            {
                return 1;
            }
            break;
        case 7:
            if(y>=extraSpaceY*2 || x<=minX)
            {
                return 1;
            }
            break;
        case 8:
            if(y>=extraSpaceY*2 || x>=extraSpaceX*2)
            {
                return 1;
            }
            break;
        case 9:
            if(y<=minY/2 || x<=minX/2 || x>= dx-minX/2)
            {
                return 1;
            }
    }
    return 0;
}



