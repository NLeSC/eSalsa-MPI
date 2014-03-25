#!/bin/bash

SHARED=../shared
BACKEND=`pwd`

#WA=wa/sockets
WA=wa/mpi

MPICC=mpicc         

echo using `which $MPICC`

rm -f *.o
rm -f ../../lib/libempi-backend.a

gfortran -c -fPIC logical.f

$MPICC -c -g -Wall -I$SHARED -fPIC debugging.c
$MPICC -c -g -Wall -I$SHARED -fPIC datatype.c
$MPICC -c -g -Wall -I$SHARED -fPIC communicator.c
$MPICC -c -g -Wall -I$SHARED -fPIC request.c
$MPICC -c -g -Wall -I$SHARED -fPIC group.c
$MPICC -c -g -Wall -I$SHARED -fPIC operation.c
$MPICC -c -g -Wall -I$SHARED -fPIC status.c
$MPICC -c -g -Wall -I$SHARED -fPIC reduce_ops.c
$MPICC -c -g -Wall -I$SHARED -fPIC file.c
$MPICC -c -g -Wall -I$SHARED -fPIC info.c
$MPICC -c -g -Wall -I$SHARED -fPIC empi.c

$MPICC -c -g -Wall -I$SHARED -I$BACKEND -fPIC $WA/messaging.c
#$MPICC -c -g -Wall -I$SHARED -I$BACKEND -fPIC $WA/wa_sockets.c

ar -cr ../../lib/libempi-backend.a \
logical.o \
debugging.o \
datatype.o \
communicator.o \
request.o \
group.o \
operation.o \
status.o \
file.o \
info.o \
reduce_ops.o \
messaging.o \
empi.o

#wa_sockets.o \

rm -f *.o
