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
 * Implements ping-pong test to determine the latency and bandwidth of a 
 * parallel computer.
 *
 * CENG 453
 * Assignment 5 Part 3
 *
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define NUM_ITERATIONS 1000
#define GET_OTHER_ID(sourceProcess) ((sourceProcess) + 1) % 2
#define TAG 1

/* Sends the specified message from process 0 to process 1, then from process 
   1 back to process 0.  The averaged time it took to send the messages is 
   returned. */
double sendMessage(char *message, int messageSize, int id) {
  MPI_Status status;
  char *recvBuffer = (char *)malloc(messageSize * sizeof(char));
  MPI_Barrier(MPI_COMM_WORLD);
  double startTime = MPI_Wtime();

  /* Send message from process 0 to process 1 */
  if (id == 0) {
    MPI_Send(message, messageSize, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
  } else if (id == 1) {
    MPI_Recv(recvBuffer, messageSize, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, 
	     &status);
  }

  /* Send message from process 1 to process 0 */
  if (id == 1) {
    MPI_Send(message, messageSize, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
  } else if (id == 0) {
    MPI_Recv(recvBuffer, messageSize, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, 
	     &status);
  }

  double endTime = MPI_Wtime();
  free(recvBuffer);
  return (endTime - startTime) / 2;
}

int main(int argc, char *argv[]) {
  int id;           /* This process's id (rank) */
  int p;            /* The total number of processes */
  int messageSize;  /* Size of the message to be passed around in bytes */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  /* Verify input parameter received */
  if (argc != 2) {
    if (id == 0) {
      printf("Usage: %s <size of test message in bytes\n", argv[0]);
    }
    MPI_Finalize();
    exit(1);
  }

  messageSize = atoi(argv[1]);
  char *message = (char *)calloc(messageSize, sizeof(char));
  if (message == NULL) {
    printf("Could not allocate memory\n");
    MPI_Finalize();
    exit(1);
  }

  /* Start by sending a message from process 0 to process 1 */
  double totalTime = 0.0;
  double minTime = DBL_MAX;
  double time;
  int i;
  for(i = 0; i < NUM_ITERATIONS; i++) {
    time = sendMessage(message, messageSize, id);
    totalTime += time;
    if (time < minTime) {
      minTime = time;
    }
  }

  double avgCommTime = totalTime / NUM_ITERATIONS;
  
  if (id == 0) {
    double tick = MPI_Wtick();
    printf("With a message size of %d bytes, the average communication time for %d iterations was %0.10f and the min time was %0.10f, with tick=%0.10f\n", 
	   messageSize, NUM_ITERATIONS, avgCommTime, minTime, tick); 
  }

  free(message);
  MPI_Finalize();

  return 0;
}
