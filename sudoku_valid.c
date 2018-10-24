/*  Jimmy Morgan
    C3600 F18
    HW2
    17 Sept 2018
    Takes file named SudokuPuzzle.txt with 9 rows and 9 tab separated columns
    Determines if it is a valid sudoku solution using threads */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

/* global variables*/
int sudokuPuzzle[9][9];

/* boolean definition */
typedef int bool;
#define TRUE 1
#define FALSE 0

bool columns[9] = {1,1,1,1,1,1,1,1,1};
bool rows[9] = {1,1,1,1,1,1,1,1,1};
bool subgrid[9] = {1,1,1,1,1,1,1,1,1};

typedef struct
{
    int topRow; //index (0-8) of top row to be checked by worker thread
    int bottomRow; //index (0-8) of bottom row to be checked by worker thread
    int leftColumn; //index (0-8) of left column to be checked by worker thread
    int rightColumn; //index (0-8) of right column to be checked by worker thread
}elementRange;

/* Print sudoku puzzle, tab separated numbers */
void printPuzzle()
{
    int i;
    int j;
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 8; j++)
        {
            printf("%d\t", sudokuPuzzle[i][j]);
        }
        printf("%d\n", sudokuPuzzle[i][8]);
    }
}

/*determine if area in range is valid */
void* checkValid(void* varg)
{
    elementRange* area = (elementRange *) varg;
    bool valid = TRUE;
    int check[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int top = area->topRow;
    int bot = area->bottomRow;
    while(top <= bot)
    {
        int left = area->leftColumn;
        int right = area->rightColumn;
        while(left <= right)
        {
            int value = sudokuPuzzle[top][left];
            if(check[value-1] == 0)
            {
                check[value-1] = 1;
            }
            else
            {
                valid = FALSE;
            }
            left++;
        }
        top++;
    }

    // Save value to correct spot in array
    if(area->leftColumn == area->rightColumn)
    {
        columns[area->rightColumn] = valid;
    }
    else if(area->bottomRow == area->topRow)
    {
        rows[area->topRow] = valid;
    }
    else if(area->topRow == 0 && area->bottomRow == 2)
    {
        switch(area->leftColumn)
        {
            case 0:
                subgrid[0] = valid;
		break;
            case 3:
                subgrid[1] = valid;
		break;
            case 6:
                subgrid[2] = valid;
		break;
        }
    }
    else if(area->topRow == 3 && area->bottomRow == 5)
    {
        switch(area->leftColumn)
        {
            case 0:
                subgrid[3] = valid;
		break;
            case 3:
                subgrid[4] = valid;
		break;
            case 6:
                subgrid[5] = valid;
		break;
        }
        
    }
    else if(area->topRow == 6 && area->bottomRow == 8)
    {
        switch(area->leftColumn)
        {
            case 0:
                subgrid[6] = valid;
		break;
            case 3:
                subgrid[7] = valid;
		break;
            case 6:
                subgrid[8] = valid;
		break;
        }
        
    }
    printf("%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d ", pthread_self(), area->topRow, area->bottomRow, area->leftColumn, area->rightColumn);
    if(valid)
    {
        printf("valid!\n");
    }
    else
    {
        printf("invalid!\n");
    }
}


/*  opens file, reads into sudokuPuzzle*/
int main(int argc, char const *argv[])
{
    FILE *fp;
    fp = fopen("./SudokuPuzzle.txt","r");

    if(fp == NULL)
    {
        printf("File SudokuPuzzle.txt not found in current directory");
        exit(1);
    }

    char line[50];
    int linenum = 0;
    char delim[] = "\t\n";
    while(fgets(line,50,fp) != NULL)
    {
        char *nums = strtok(line,delim);
        int i = 0;
        while(nums != NULL)
        {
            sudokuPuzzle[linenum][i] = atoi(nums);
            nums = strtok(NULL,delim);
            i++;
        }        
        linenum++;
    }
    fclose(fp);
    printPuzzle();

    /*Making Threads*/
    pthread_t tid;

    /* columns */
    pthread_t tid_column[9];
    elementRange ranges[9];
    int i;
    for(i = 0; i < 9; i++)
    {
        ranges[i].topRow = 0;
        ranges[i].bottomRow = 8;
        ranges[i].leftColumn = i;
        ranges[i].rightColumn = i;
    }

    for(i = 0; i < 9; i++)
    {
        pthread_create(&tid, NULL, checkValid, (void *) &ranges[i]);
        tid_column[i] = tid;
    }

    /* rows*/
    pthread_t tid_row[9];
    elementRange len[9];
    for(i = 0; i < 9; i++)
    {
        len[i].topRow = i;
        len[i].bottomRow = i;
        len[i].leftColumn = 0;
        len[i].rightColumn = 8;
    }

    for(i = 0; i < 9; i++)
    {
        pthread_create(&tid, NULL, checkValid, (void *) &len[i]);
        tid_row[i] = tid;
    }

    /* Subgrids */
    pthread_t tid_subgrid[9];
    elementRange grids[9];
    int gridnum = 0;
    int j;
    for(i = 0; i < 9; i=i+3)
    {
        for(j = 0; j < 9; j=j+3)
        {
            grids[gridnum].topRow = j;
            grids[gridnum].bottomRow = j+2;
            grids[gridnum].leftColumn = i;
            grids[gridnum].rightColumn = i+2;
            gridnum++;
        }
    }

    for(i = 0; i < 9; i++)
    {
        pthread_create(&tid, NULL, checkValid, (void *) &grids[i]);
        tid_subgrid[i] = tid;
    }

    /*Wait*/
    for(i = 0; i < 9; i++)
    {
        pthread_join(tid_column[i],NULL);
        pthread_join(tid_row[i],NULL);
        pthread_join(tid_subgrid[i],NULL);
    }

    /* print results*/
    bool valid = TRUE;
    for(i = 0; i < 9; i++)
    {
        printf("Column: %lx ", tid_column[i]);
        if(columns[i])
        {
            printf("valid\n");
        }
        else
        {
            printf("invalid\n");
            valid = FALSE;
        }
    }

    for(i = 0; i < 9; i++)
    {
        printf("Row: %lx ", tid_row[i]);
        if(rows[i])
        {
            printf("valid\n");
        }
        else
        {
            printf("invalid\n");
            valid = FALSE;
        }
    }

    for(i = 0; i < 9; i++)
    {
        printf("Subgrid: %lx ", tid_subgrid[i]);
        if(subgrid[i])
        {
            printf("valid\n");
        }
        else
        {
            printf("invalid\n");
            valid = FALSE;
        }
    }

    printf("Sudoku Puzzle: ");
    if(valid)
    {
        printf("valid\n");
    }
    else
    {
        printf("invalid\n");
    }

    return 0;
}
