#!/bin/bash

#module purge
#module list

#MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

CC=gcc

rm -f *.o libempi.a libempi.so

$CC -c -g -Wall -fPIC logging.c
$CC -c -g -Wall -fPIC profiling.c
$CC -c -g -Wall -fPIC empi.c
$CC -c -g -Wall -fPIC empif.c

$CC -shared -o libempi.so \
logging.o \
profiling.o \
empi.o \
empif.o

ar -cr libempi.a \
logging.o \
profiling.o \
empi.o \
empif.o

rm -f *.o

