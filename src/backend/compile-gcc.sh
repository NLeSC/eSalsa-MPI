#!/bin/bash

#module purge
#module list

#MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

SHARED=../shared

MPI=/usr/lib/openmpi
MPICC=/usr/bin/mpicc

echo using `which $MPICC`

rm -f *.o
rm -f ../../lib/libempi-backend.a

gfortran -c -fPIC logical.f

$MPICC -c -g -Wall -I$SHARED -fPIC debugging.c
$MPICC -c -g -Wall -I$SHARED -fPIC datatype.c
$MPICC -c -g -Wall -I$SHARED -fPIC communicator.c
$MPICC -c -g -Wall -I$SHARED -fPIC request.c
$MPICC -c -g -Wall -I$SHARED -fPIC messaging.c
$MPICC -c -g -Wall -I$SHARED -fPIC group.c
$MPICC -c -g -Wall -I$SHARED -fPIC operation.c
$MPICC -c -g -Wall -I$SHARED -fPIC status.c
$MPICC -c -g -Wall -I$SHARED -fPIC reduce_ops.c
$MPICC -c -g -Wall -I$SHARED -fPIC wa_sockets.c
$MPICC -c -g -Wall -I$SHARED -fPIC file.c
$MPICC -c -g -Wall -I$SHARED -fPIC info.c
$MPICC -c -g -Wall -I$SHARED -fPIC empi.c

ar -cr ../../lib/libempi-backend.a \
logical.o \
debugging.o \
datatype.o \
communicator.o \
request.o \
messaging.o \
group.o \
operation.o \
status.o \
file.o \
info.o \
reduce_ops.o \
wa_sockets.o \
empi.o

rm -f *.o

