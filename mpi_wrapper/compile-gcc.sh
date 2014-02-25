#!/bin/bash

#module purge
#module list

#MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

MPI=/usr/lib/openmpi
MPICC=/usr/bin/mpicc

echo using `which $MPICC`

rm -f *.o
rm -f ../lib/libempi-backend.a
rm -f ../lib/libempi-backend.so

gfortran -c -fPIC logical.f

$MPICC -c -g -Wall -fPIC logging.c
$MPICC -c -g -Wall -fPIC debugging.c
$MPICC -c -g -Wall -fPIC datatype.c
$MPICC -c -g -Wall -fPIC communicator.c
$MPICC -c -g -Wall -fPIC request.c
$MPICC -c -g -Wall -fPIC messaging.c
$MPICC -c -g -Wall -fPIC group.c
$MPICC -c -g -Wall -fPIC operation.c
$MPICC -c -g -Wall -fPIC status.c
$MPICC -c -g -Wall -fPIC reduce_ops.c
$MPICC -c -g -Wall -fPIC wa_sockets.c
$MPICC -c -g -Wall -fPIC empi.c

ar -cr ../lib/libempi-backend.a \
logical.o \
logging.o \
debugging.o \
datatype.o \
communicator.o \
request.o \
messaging.o \
group.o \
operation.o \
status.o \
reduce_ops.o \
wa_sockets.o \
empi.o

rm -f *.o

