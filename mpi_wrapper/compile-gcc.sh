#!/bin/bash

#module purge
#module list

#MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu

MPI=/usr/lib/openmpi
MPICC=/usr/bin/mpicc

echo using `which $MPICC`

rm -f *.o libempi.a libempi.so

gfortran -c -fPIC logical.f

$MPICC -c -g -Wall -fPIC logging.c
$MPICC -c -g -Wall -fPIC debugging.c
$MPICC -c -g -Wall -fPIC profiling.c
$MPICC -c -g -Wall -fPIC datatype.c
$MPICC -c -g -Wall -fPIC communicator.c
$MPICC -c -g -Wall -fPIC request.c
$MPICC -c -g -Wall -fPIC messaging.c
$MPICC -c -g -Wall -fPIC group.c
$MPICC -c -g -Wall -fPIC operation.c
$MPICC -c -g -Wall -fPIC status.c
$MPICC -c -g -Wall -fPIC reduce_ops.c
$MPICC -c -g -Wall -fPIC wa_sockets.c
$MPICC -c -g -Wall -fPIC xempi.c
$MPICC -c -g -Wall -fPIC empi.c
$MPICC -c -g -Wall -fPIC empif.c

$MPICC -shared -o libempi.so \
xempi.o \
empi.o \
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
empif.o


ar -cr libempi.a \
xempi.o \
empi.o \
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
empif.o

rm -f *.o

