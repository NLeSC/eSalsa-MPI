#!/bin/bash

#module purge
#module list

#MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

CC=gcc
INC=../../include

OPT=-O3

rm -f *.o
rm -f ../../lib/libempi-frontend.a

$CC -c $OPT -Wall -fPIC -I$INC empi_wrapper.c
$CC -c $OPT -Wall -fPIC -I$INC empif_wrapper.c

ar -cr ../../lib/libempi-frontend.a \
empif_wrapper.o \
empi_wrapper.o

# rm -f *.o

