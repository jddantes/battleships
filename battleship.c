#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define OS 1 

typedef struct{
    int j;
    int i;
}coord_t;

typedef struct{
    int id;
    int size;
    int afloat;
    coord_t * parts;
    int * parts_state;
    char ship_name[26];
}ship_t;

typedef struct{
    int ship_id;
    coord_t coord;
}row_t;

int search_ship(coord_t coord, row_t table[], int table_size);
int coordcmp(coord_t p, coord_t q);
int numDig(int n);
void clear_screen();
void delay();

int main(int argc, char ** argv)
{
        printf("OS is %d\n",OS);

        FILE * fp = NULL;
        if(argc == 1)
        {
            fp = fopen("battleship.txt","r");
        }
        else if (argc == 2)
        {
            fp = fopen(argv[1],"r");
        }
        else
        {
            printf("Invalid options.\n");
            return 0;
        }

        if(fp == NULL)
        {
            printf("Failed to open file.\n");
            return 0;
        }

        int size;
        fscanf(fp,"%d",&size);
        int min,max;
        fscanf(fp,"%d-%d",&min,&max);
        if(max>size)
        {
            printf("Max is greater than size.\n");
            printf("   max  : %d\n",max);
            printf("   size : %d\n",size);
            return 0;
        }
        int num_ships = max-min + 1;
        char grid[size][size];
        int target[size][size];
        memset(target,0,size*size*sizeof(int));

        int sunk = 0;

        /*
            Get the contents of the file, then close it.
        */
        int i, j;
        for(i=0; i<size; i++)
        {
            for(j=0; j<size; j++)
            {
                fscanf(fp," %c",&grid[i][j]);
                if(grid[i][j]=='.')
                {
                    grid[i][j] = 0;
                }
            }
        }

        int ship_name_len = sizeof(char*)*sizeof(((ship_t){}).ship_name);

        /*
            Initialize list of ship_names
        */
        char ** ship_names = (char **)malloc(sizeof(char*)*num_ships);
        for(i=0;i<num_ships;i++)
        {
            ship_names[i] = (char *)malloc(sizeof(char)*ship_name_len);
            strcpy(ship_names[i],"");
        }

        char f_buffer[40];
        /*
            Get ship names, if any.
        */
        fgets(f_buffer,40,fp); //Clear out remaining newline at the end of scanning grid
        while(fgets(f_buffer,40,fp)!=NULL)
        {
            f_buffer[strlen(f_buffer) - 1] = 0; //remove newline of f_buffer scanned by fgets
            int curr_size;
            sscanf(f_buffer,"%d",&curr_size);

            if(curr_size<min || curr_size > max)
            {
                printf("Size of ship (%d) not in specified range (%d-%d).\n",curr_size,min,max);
                printf("Found in file:\n");
                printf("\t%s",f_buffer);
                printf("Aborting...\n");
                return 0;
            }

            int ship_id = curr_size - min;
            strncpy(ship_names[ship_id],f_buffer+numDig(curr_size)+1,ship_name_len);
            //printf("Ship %d with %d parts is called %s\n",ship_id,curr_size,ship_names[ship_id]);
        }

        fclose(fp);

        //Check ship_names
        /*
        for(i=0; i<num_ships; i++)
        {
            printf("Ship %d is named %s\n",i,ship_names[i]);
        }*/

        //printf("Ships done.\n");

        /*
        ***** START CONSTRUCTING SHIPS *****
        */

        /*
            Have a table to easily look up which ship is at a given position.
        */
        int table_size = num_ships*(max+min)/2;
        row_t table[table_size];
        int table_index = 0;

        ship_t ships[num_ships];

        int e=0;
        int curr_size = min;
        /*
            For each ship...
        */
        for(e; e<num_ships; e++, curr_size++)
        {
            //printf("Searching for ship %d\n",e);

            ships[e].id = e;
            ships[e].size = curr_size;
            ships[e].afloat = curr_size;
            ships[e].parts = (coord_t *)malloc(sizeof(coord_t) * curr_size);
            ships[e].parts_state = (int *)malloc(sizeof(int)*curr_size);
            strcpy(ships[e].ship_name,ship_names[e]);
            /*
                Get the location of each segment, stored in coord_t array parts.
                Do this by searching through the whole grid.
            */

            char curr_char = 'A' + e;
            /*
                The first ship (the one with the smallest size) will be marked by the first letter, i.e. 'A',
                the second with 'B', and so on.
            */

            int parts_counted = 0;
            /*
                A variable to keep track if all of the ship has been found, so we don't need to continue searching
                through the grid.
                It also serves as index for the parts array.
            */

            /*
                Search the whole grid:
            */
            for(i=0; i<size; i++)
            {
                if(parts_counted == curr_size)
                {
                    break;
                }
                for(j=0; j<size; j++)
                {


                    if(grid[i][j] == curr_char)
                    {
                        //printf("Found %c at %d,%d\n",curr_char,j,i);

                        ships[e].parts[parts_counted] = (coord_t) {j,i};
                        ships[e].parts_state[parts_counted] = 1;
                        parts_counted++;

                        /*
                            Update table to reflect found position.
                        */
                        table[table_index] = (row_t){e,(coord_t){j,i}};
                        table_index++;

                        if(parts_counted == curr_size)
                        {
                            //printf("All %d accounted for.\n",parts_counted);
                            break;
                        }
                    }
                }
                if(parts_counted == curr_size)
                {
                    break;
                }
            }
        }

        //Check ships:
        /*
        for(i=0; i<num_ships; i++)
        {
            printf("Ship %d: %s\n",ships[i].id,ships[i].ship_name);
            for(j=0; j<ships[i].size; j++)
            {
                printf("\t%d %d,%d\n",ships[i].parts_state[j],ships[i].parts[j].j,ships[i].parts[j].i);
            }
            printf("Total parts afloat: %d\n\n",ships[i].afloat);
        }*/

        //Print table
        /*
        for(i=0; i<table_size; i++)
        {
            printf("%d (%d,%d)\n",table[i].ship_id, table[i].coord.j, table[i].coord.i);
        }*/

        /*
        ***** START GETTING INPUT *****
        */

        while(1)
        {
            clear_screen();

            //Print the board with enemy vision
            /*
                Also what will be sent to the server.
            */
            printf("What your rival sees:\n");
            for(i=0; i<size; i++)
            {
                for(j=0; j<size; j++)
                {
                    if(!grid[i][j])
                    {
                        printf(". ");
                    }
                    else
                    {
                        int ship_id = search_ship((coord_t){j,i},table,table_size);
                        if(ship_id == -1)
                        {
                            printf("search_ship failed.\n");
                            printf("This shouldn't happen.\n");
                            printf("Aborting...\n");
                            return 0;
                        }

                        /*
                        ***** SEARCH FOR PART INDEX ******
                        */
                        int part_index = -1;
                        int x;
                        for(x=0; x<ships[ship_id].size; x++)
                        {
                            if(!coordcmp((coord_t)ships[ship_id].parts[x], (coord_t){j,i}))
                            {
                                part_index = x;
                            }
                        }

                        if(part_index == -1)
                        {
                            printf("part_index wasn't found.\n");
                            printf("This shouldn't happen.\n");
                            printf("Aborting...\n");
                            return 0;
                        }

                        /*
                            The part above is used to determine if the part of the boat has been sunk or not.
                        */

                        if(ships[ship_id].parts_state[part_index])
                        {
                            /*
                                If part is still afloat, print normally. (CAPITAL)
                            */
                            printf("%c ",'A'+ship_id);
                        }
                        else
                        {
                            /*
                                Else print as lowercase.
                            */
                            printf("%c ",tolower('A'+ship_id));
                        }


                    }
                }
                printf("\n");
            }

            /*
            ***** PRINT SHIP STATUS *****
            */
            printf("\nSTATUS\n\n");
            printf("Ships afloat: %d\n",num_ships - sunk);
            printf("Ships sunk: %d\n\n",sunk);
            for(i=0; i<num_ships; i++)
            {
                //printf("ID is %02d ASDFASDF",ships[i].id);
                /*
                    HELP
                    Rambos on linux. Gets fixed with newline for some reason.
                    printf("ID is %02d ASDFASDF\n");
                */
                printf("%02d",ships[i].id);

                if(strlen(ships[i].ship_name))
                    printf(" %s",ships[i].ship_name);
                else
                    printf("");
                if(ships[i].afloat == 0)
                {
                    printf(" [SUNK]  \n");
                }
                else
                {
                    printf(" [ONLINE] \n");
                }
                printf("   %d/%d REMAINING\n",ships[i].afloat,ships[i].size);

                for(j=0; j<ships[i].size; j++)
                {
                    printf("   ");
                    //printf("(%d,%d) ",ships[i].parts[j].j, ships[i].parts[j].i);
                    printf("(%d,%d) ",ships[i].parts[j].j, size - 1 - ships[i].parts[j].i); //See declaration of yp below
                    if(ships[i].parts_state[j])
                    {
                        printf("GOOD\n");
                    }
                    else
                    {
                        printf("HIT\n");
                    }
                }

                printf("\n");
            }
            /*
            ***** PRINT THE BOARD ***
            */
            printf("What you see:\n");

            //Print target
            for(i=0; i<size; i++)
            {
                for(j=0; j<size; j++)
                {
                    if(!target[i][j])
                    {
                        printf(". ");
                    }
                    else if(target[i][j] == 1) //HIT!!!
                    {
                        printf("X ");
                    }
                    else if(target[i][j] == -1)//BLANK
                    {
                        printf("  ");
                    }
                    else
                    {
                        printf("Error in reading target.\n");
                        printf("This shouldn't be happening.\n");
                        printf("Aborting...\n");
                        return 0;
                    }
                }
                printf("\n");
            }

            int x, y;
            char x_c;
            /*
                (0,0) is at i = size - 1, j = 0
                That is, the grid is the first quadrant of the Cartesian plane, with
                    the lower left corner as the origin.
            */
            char s_buff[15];
            int repeat = 1;
            int xp, yp; //See comment about coordinates above. xp corresponds to j in grid, and yp to i.
            do
            {
                printf("Enter coordinates: ");
                fgets(s_buff,15,stdin);
                sscanf(s_buff," %d %d",&x,&y);

                if(x<0 || y<0 || x>=size || y>=size )
                {
                    printf("Out of bounds. Coordinates must be between 0 and %d (inclusive)\n",size-1);
                    continue;
                }
                xp = x;
                yp = (size - 1) - y; //See comment about the coordinates above.

                if(target[yp][xp]) //Point already inspected
                {
                    printf("Point already inspected.\n");
                }
                else
                {
                    repeat = 0;
                }
            }while(repeat);

            /*
            printf("You're looking at (%d,%d)\n",x,y);
            printf("That is, %d %d\n",xp,yp);
            printf("Grid value: %d\n",grid[yp][xp]);
            */


            if(grid[yp][xp]) //Ship found
            {
                int ship_id = search_ship((coord_t){xp,yp},table,table_size);
                if(ship_id == -1)
                {
                    printf("search_ship failed!\nAborting...");
                    return 0;
                }

                printf("It's a hit!\n");

                        /*
                        ***** SEARCH FOR PART INDEX ******
                        */

                        int part_index = -1;
                        int x;
                        for(x=0; x<ships[ship_id].size; x++)
                        {
                            if(!coordcmp((coord_t)ships[ship_id].parts[x], (coord_t){xp,yp}))
                            {
                                part_index = x;
                            }
                        }

                        if(part_index == -1)
                        {
                            printf("part_index wasn't found.\n");
                            printf("This shouldn't happen.\n");
                            printf("Aborting...\n");
                            return 0;
                        }
                    ships[ship_id].parts_state[part_index] = 0;
                    ships[ship_id].afloat--;
                    if(ships[ship_id].afloat == 0) //Ship was sunk
                    {
                        if(!strcmp(ships[ship_id].ship_name,""))
                        {
                            printf("You sunk the enemy's ship!\n");
                        }
                        else
                        {
                            printf("You sunk your enemy's %s!\n",ships[ship_id].ship_name);
                        }

                        sunk++;
                        if(sunk == num_ships) //All ships were sunk
                        {
                            printf("You won! All ships were sunk!\n");

                            /*
                            ***** FREE MEMORY *****
                            */
                            for(e=0;e<num_ships;e++)
                            {
                                free(ship_names[e]);
                                free(ships[e].parts);
                                free(ships[e].parts_state);
                            }
                            free(ship_names);
                            return 0;
                        }
                    }

                target[yp][xp] = 1;
            }
            else //Point is blank
            {
                printf("It appears to have missed.\n");
                target[yp][xp] = -1;
            }

            //delay();
            getchar();
        }



    return 0;
}

int search_ship(coord_t coord, row_t table[], int table_size)
{
    int i;
    for(i=0; i<table_size; i++)
    {
        /*
        ***** HELP!!! *****
            Can't use coord == table[i].coord
                Invalid operands to binary of type 'coord_t' and 'coord_t'
            memcmp(coord, table[i].coord) needed to be replaced with own implementation (coordcmp)
                Program stops working
        */
        //printf("Comparing %d,%d to table record %d,%d\n",coord.j,coord.i,table[i].coord.j,table[i].coord.i);

        if(!coordcmp(coord, table[i].coord))
        {
            return table[i].ship_id;
        }
    }

    printf("search_ship failed to look for coord(%d,%d)\n",coord.j,coord.i);
    return -1; //Ship not found
}

int coordcmp(coord_t p, coord_t q)
{
    if(p.j == q.j && p.i == q.i)
    {
        return 0;
    }
    return 1;
}

int numDig(int n)
{
    if(!n)
    {
        return 1;
    }
    int cnt = 0;
    if(n<0)
    {
        n = -n;
    }
    while(n>0)
    {
        n/=10;
        cnt++;
    }
    return cnt;
}

void clear_screen()
{
    /*
        Clear-screen dependent on OS
            0 Linux
            1 Windows
    */
    switch(OS)
    {
        case 0:
            system("clear");
            break;
        case 1:
            system("cls");
            break;
    }
}

void delay()
{
    int i;
    for(i=0; i<400000000; i++)
    {

    }
}
