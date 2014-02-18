#!/bin/bash

#module purge
#module list

#MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

MPI=/usr/lib/openmpi
MPICC=/usr/bin/mpicc

echo using `which $MPICC`

rm -f *.o .libempi.a

gfortran -c logical.f

$MPICC -c -g -Wall logging.c
$MPICC -c -g -Wall debugging.c
$MPICC -c -g -Wall profiling.c
$MPICC -c -g -Wall datatype.c
$MPICC -c -g -Wall communicator.c
$MPICC -c -g -Wall request.c
$MPICC -c -g -Wall messaging.c
$MPICC -c -g -Wall group.c
$MPICC -c -g -Wall operation.c
$MPICC -c -g -Wall status.c
$MPICC -c -g -Wall reduce_ops.c
$MPICC -c -g -Wall wa_sockets.c
$MPICC -c -g -Wall xempi.c
$MPICC -c -g -Wall empi.c
$MPICC -c -g -Wall empif.c

ar -cr libempi.a \
logical.o \
logging.o \
debugging.o \
profiling.o \
datatype.o \
communicator.o \
request.o \
messaging.o \
group.o \
operation.o \
status.o \
reduce_ops.o \
wa_sockets.o \
xempi.o \
empi.o \
empif.o

rm -f *.o
