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
 * Serial implementation of the boundary value problem
 * Described in Section 3.4 of text
 *
 * David Rusk
 *
 * CENG 453
 * Assignment 3
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265

/* These values will be read from the command line */
int numSegments;
double rVal;
int numIterations;
int numOutputs;

/*
  Checks whether the right number of arguments were passed in.  If it
  wasn't the right number, a usage message is displayed and the program
  exits with an error code 1. 
*/
void validateArgCount(int argc) {
  if (argc != 5) {
    printf("Invalid number of arguments\n");
    printf("Usage: <executable name> <# of segments> <r value> <# of iterations> <# output>\n");
    printf("Example: ./bv 100 0.1 50000 5\n");
    exit(1);
  }
}

/*
  Parses an integer from an input character array.
*/
int parseInt(char *input) {
  return strtol(input, NULL, 10);
}

/* 
   Parses a double from an input character array.
*/
double parseDouble(char *input) {
  return strtod(input, NULL);
}

int main(int argc, char **argv) {
  validateArgCount(argc);
  numSegments = parseInt(argv[1]);
  rVal = parseDouble(argv[2]);
  numIterations = parseInt(argv[3]);
  numOutputs = parseInt(argv[4]);

  printf("Number of segments = %d\n", numSegments);
  printf("R value = %f\n", rVal);
  printf("Number of iterations = %d\n", numIterations);
  printf("Number of outputs = %d\n", numOutputs);

  /* Each row holds the results along the rod for a single iteration */
  double tempOverTime[numIterations][numSegments];

  /* Initialize temperatures */
  int i;
  for (i = 0; i < numSegments; i++) {
    printf("i=%d\n", i);
    tempOverTime[0][i] = 100*sin(PI*i/numSegments);
  }

  /* Initialization is counted as an iteration */
  int iteration;
  for (iteration = 1; iteration < numIterations; iteration++) {
    /* For each iteration, calculate the new temperature values along the rod */
    int i;
    for (i = 0; i < numSegments; i++) {
      tempOverTime[iteration][i] = (1 - 2*rVal) * tempOverTime[iteration-1][i];
      if (i > 0) {
	tempOverTime[iteration][i] += rVal * tempOverTime[iteration-1][i-1];
      }
      if (i < numSegments-1) {
	tempOverTime[iteration][i] += rVal * tempOverTime[iteration-1][i+1];
      }
    }
  }

  int samplingIncrement = numIterations / numOutputs;
  for (i = 0; i < numIterations; i+=samplingIncrement) {
    int j;
    printf("Sampling iteration %d\n", i);
    for (j = 0; j < numSegments; j++) {
      printf("%f ", tempOverTime[i][j]);
    }
    printf("\n");
  }

  return 0;
}
