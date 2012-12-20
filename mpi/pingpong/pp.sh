#!/bin/sh
mpirun -np 2 /home/drusk/ceng453/a5/p2/pingpong 1
mpirun -np 2 /home/drusk/ceng453/a5/p2/pingpong 8
mpirun -np 2 /home/drusk/ceng453/a5/p2/pingpong 64
mpirun -np 2 /home/drusk/ceng453/a5/p2/pingpong 1024
mpirun -np 2 /home/drusk/ceng453/a5/p2/pingpong 8192
mpirun -np 2 /home/drusk/ceng453/a5/p2/pingpong 32768
mpirun -np 2 /home/drusk/ceng453/a5/p2/pingpong 65536

