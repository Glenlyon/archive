/******************************************************************************
* Copyright (C) 2012 David Rusk
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*****************************************************************************/

/*
 * Parallel implementation of the game of Life.
 *
 * CENG 453
 * Assignment 5 Part 3
 *
 */

#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define GRID_ROWS 1000
#define GRID_COLUMNS 1000
#define ITERATIONS 10000

/* Each cell is in one of these two states */
#define LIVE 1
#define DEAD 0

#define	WRAP(val, maxVal) ((val) < 0 ? (maxVal) + (val) : (val) % (maxVal))

#define TOP_ROW(pid) ((pid) * (GRID_ROWS / p))
#define BOTTOM_ROW(pid) (TOP_ROW((pid) + 1) - 1)
#define NUM_ROWS(pid) (BOTTOM_ROW(pid) - TOP_ROW(pid) + 1)

/* Tags for MPI_Send and MPI_Recv */
#define SHARE_BOTTOM 0
#define SHARE_TOP 1

#define N_SIZE 8 /* The number of elements in the neighbourhood */

/* Process-wide data */
int id;   /* Current process's identification rank */
int p;    /* Total number of processes */

/* Counts the number of "live" (value 1) cells in the 8-neighbourhood 
   surrounding cell (x, y). */
int countLiveNeighbours(char **grid, char *rowAbove, char *rowBelow, 
			int targetRow, int targetColumn) {
  int count = 0;
  /* Check each of the 8 neighbours starting at the top left and working 
     clockwise.  For x or y at an edge, the neighbourhood wraps around to the 
     other side of the grid. */
  int columnOffsets[N_SIZE] = { -1, 0, 1, 1, 1, 0, -1, -1 };
  int rowOffsets[N_SIZE] = { -1, -1, -1, 0, 1, 1, 1, 0 };
  int i;
  for (i = 0; i < N_SIZE; i++) {
    /* Get unwrapped (raw) coordinates of neighbour */
    int rawColumn = targetColumn + columnOffsets[i];
    int rawRow = targetRow + rowOffsets[i];
    /* Wrap if coordinates too big or small */
    int neighbourColumn = WRAP(rawColumn, GRID_COLUMNS);

    /* Wrapping in the y direction (rows) requires looking at the neighbour 
       processes' shared rows. */
    if (rawRow == -1) {
      if(rowAbove[neighbourColumn] == LIVE) {
	count++;
      } 
    }
    else if (rawRow == NUM_ROWS(id)) {
      if(rowBelow[neighbourColumn] == LIVE) {
	count++;
      }
    }
    else if (grid[rawRow][neighbourColumn] == LIVE) {
      count++;
    }
  }
  return count;
}

/* Determines a cell's state for the next iteration given its current state 
   and number of live neighbours. */
char getNextState(char currentState, int numLiveNeighbours) {
  if ((currentState == DEAD && numLiveNeighbours == 3) ||
      (currentState == LIVE && (numLiveNeighbours == 2 || 
				numLiveNeighbours == 3))) {
    return LIVE;
  }
  return DEAD;
}

/* Creates grid but does not initialize its values. */
char **createGrid() {
  int numRows = NUM_ROWS(id);
  /* Allocate the memory for the 2d grid in one continuous chunk */
  char *gridStorage = (char *)malloc(numRows * GRID_COLUMNS * sizeof(char));

  /* Now create the pointers that point to the start of each row.  This 
     simulates a 2d grid even though the actual data is stored in 1d. */
  char **grid = (char **)malloc(numRows * sizeof(char *));
  int i;
  for (i = 0; i < numRows; i++) {
    grid[i] = &gridStorage[i * GRID_COLUMNS];
  }

  return grid;
}

/* Initiallizes this process's portion of the grid.  Each element has a 40% 
   chance of being initialized as LIVE.  The rest start DEAD. */
char **initGrid() {
 char **grid = createGrid();
  
  /* Initialize random number generator */
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);

  /* Set initial values of grid elements */
  int row;
  int column;
  int numRows = NUM_ROWS(id);
  for (row = 0; row < numRows; row++) {
    for (column = 0; column < GRID_COLUMNS; column++) {
      /* Generate a random number between 0 and 1 */
      double randNum = (double)rand() / RAND_MAX;
      /* 40% chance of being set as alive */
      grid[row][column] = randNum < 0.4 ? LIVE : DEAD;
    }
  }

  return grid;
}

void freeGrid(char **grid) {
  free(grid[0]); /* Frees the "storage" array */
  free(grid);    /* Frees the row pointer array */
}

/* Updates the grid to its state for the next iteration. */
char **computeIteration(char **grid, char *rowAbove, char *rowBelow) {
  char **newGrid = createGrid();
  int row;
  int column;
  int numRows = NUM_ROWS(id);
  for (row = 0; row < numRows; row++) {
    for (column = 0; column < GRID_COLUMNS; column++) {
      int numLiveNeighbours = countLiveNeighbours(grid, rowAbove, rowBelow, 
						  row, column);
      char currentState = grid[row][column];
      newGrid[row][column] = getNextState(currentState, numLiveNeighbours);
    }
  }
  freeGrid(grid);
  return newGrid;
}

/* Each process needs a row from the process before and after it in order to 
   compute each iteration.  The process is given pointers to these rows via 
   rowAbovePtr and rowBelowPtr. */
void shareBorderRows(char **grid, char **rowAbovePtr, char **rowBelowPtr) {

  /* Special case when running with only 1 processor: it actually contains all 
     the rows it needs, just needs to wrap around. */
  if (p == 1) {
    *rowAbovePtr = grid[GRID_ROWS - 1];
    *rowBelowPtr = grid[0];
    return;
  }

  /* Get ids of processes to communicate with */
  int processAbove = WRAP(id - 1, p);
  int processBelow = WRAP(id + 1, p);

  /* Send this process's first row to the process above it */
  MPI_Send(grid[0], GRID_COLUMNS, MPI_CHAR, processAbove, SHARE_TOP, 
	   MPI_COMM_WORLD);
  /* Send this process's last row to the process below it */
  MPI_Send(grid[NUM_ROWS(id) - 1], GRID_COLUMNS, MPI_CHAR, processBelow, 
	   SHARE_BOTTOM, MPI_COMM_WORLD);

  MPI_Status recvAboveStat;
  MPI_Status recvBelowStat;

  /* Receive the process below's first row */
  MPI_Recv(*rowBelowPtr, GRID_COLUMNS, MPI_CHAR, processBelow, SHARE_TOP, 
	   MPI_COMM_WORLD, &recvBelowStat);
  /* Receive the above process's last row */
  MPI_Recv(*rowAbovePtr, GRID_COLUMNS, MPI_CHAR, processAbove, SHARE_BOTTOM, 
	   MPI_COMM_WORLD, &recvAboveStat);
}

/* Prints the grid for debugging purposes.  Should only be used for small
   grid sizes and small number of iterations. */
void printGrid(char **grid) {
  char output[NUM_ROWS(id) * GRID_COLUMNS + GRID_ROWS + 1];
  int row;
  int column;
  printf("PID=%d", id);
  printf("\n");
  for (row = 0; row < NUM_ROWS(id); row++) {
    for (column = 0; column < GRID_COLUMNS; column++) {
      if (grid[row][column] == LIVE) {
	printf("%d", 1);
      }
      else if (grid[row][column] == DEAD) {
	printf("%d", 0);
      }
      else {
	printf("%d", 2);
      }
    }
    printf("\n");
  }

  int i;
  for (i = 0; i < GRID_COLUMNS; i++) {
    printf("*");
  }
  printf("\n");
}

/* Returns the number of cells in this process's grid which are classified as 
   LIVE */
int countLivingCells(char **grid) {
  int count = 0;
  int row;
  int column;
  int numRows = NUM_ROWS(id);
  for (row = 0; row < numRows; row++) {
    for (column = 0; column < GRID_COLUMNS; column++) {
      if (grid[row][column] == LIVE) {
	count++;
      }
    }
  }
  return count;
}

int main(int argc, char *argv[]) {
  /* MPI startup */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  MPI_Barrier(MPI_COMM_WORLD);
  double startTime = MPI_Wtime();

  /* The number of live cells in the grid managed by this process */
  int localPopulation;  

  /* The total number of living cells across all processes */
  int globalPopulation;

  /* Create this process's portion of the grid */
  char **grid = initGrid();

  /* Need to communicate with other processes for neighbours of the top and 
     bottom rows */
  char *rowAbove;
  char *rowBelow;
  if (p != 1) {
    /* Need to allocate some extra storage space if this process doesn't have 
       the full grid because it will have to receive some extra data from 
       its neighbours above and below. */
    rowAbove = (char *)malloc(GRID_COLUMNS * sizeof(char));
    rowBelow = (char *)malloc(GRID_COLUMNS * sizeof(char));
  }

  shareBorderRows(grid, &rowAbove, &rowBelow);

  localPopulation = countLivingCells(grid);
  MPI_Reduce(&localPopulation, &globalPopulation, 1, MPI_INT, MPI_SUM, 0, 
	     MPI_COMM_WORLD);
  if (id == 0) {
    printf("Initial population: %d\n", globalPopulation);
  }

  int i;
  for (i = 0; i < ITERATIONS; i++) {
    grid = computeIteration(grid, rowAbove, rowBelow);
    shareBorderRows(grid, &rowAbove, &rowBelow);
   
    localPopulation = countLivingCells(grid);
    MPI_Reduce(&localPopulation, &globalPopulation, 1, MPI_INT, MPI_SUM, 0, 
	       MPI_COMM_WORLD);
    if (id == 0) {
      printf("Population after iteration %d: %d\n", i, globalPopulation);
    }
  }

  /* Free up resources */
  if (p != 1) {
    /* Free the extra communication space that was allocated */
    free(rowAbove);
    free(rowBelow);
  }
  freeGrid(grid);

  MPI_Barrier(MPI_COMM_WORLD);
  double endTime = MPI_Wtime();
  if (id == 0) {
    printf("Total execution time: %10.6f\n", endTime - startTime);
  }

  MPI_Finalize();
  return 0;
}
