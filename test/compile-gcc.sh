#!/bin/bash

MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

echo using $MPI

module purge
module list

rm -f *.o app.exe
gcc -O3 -Wall chrongear.c -o chrongear.exe -I$MPI/include -L.. -lmpibis -L$MPI/lib -lmpi -lm
gcc -O3 -Wall border.c -o border.exe -I$MPI/include -L.. -lmpibis -L$MPI/lib -lmpi -lm

