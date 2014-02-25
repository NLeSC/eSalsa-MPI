#!/bin/bash

#module purge
#module list

#MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

CC=gcc

rm -f *.o
rm -f ../lib/libempi-frontend.a
rm -f ../lib/libempi-frontend.so

$CC -c -g -Wall -fPIC logging.c
$CC -c -g -Wall -fPIC profiling.c
$CC -c -g -Wall -fPIC empi_wrapper.c
$CC -c -g -Wall -fPIC empif_wrapper.c

ar -cr ../lib/libempi-frontend.a \
logging.o \
profiling.o \
empi_wrapper.o \
empif_wrapper.o


