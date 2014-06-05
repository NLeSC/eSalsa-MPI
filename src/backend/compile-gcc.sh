#!/bin/bash

#MPWIDE=/home/jason/Workspace/MPWide
SHARED=../shared
BACKEND=`pwd`

#WA=wa/sockets
#WA=wa/mpi

MPICC=mpicc
#GPP=g++

OPT=-O3

echo using `which $MPICC`

rm -f *.o
rm -f ../../lib/libempi-backend.a

#$GPP -c -g -Wall -I$SHARED -I$BACKEND -I$MPWIDE -fPIC $WA/mpwidec.cpp

gfortran -c -fPIC logical.f

$MPICC -c $OPT -Wall -I$SHARED -fPIC debugging.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC message_queue.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC datatype.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC communicator.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC request.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC group.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC operation.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC status.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC reduce_ops.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC file.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC info.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC empi.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC messaging.c
$MPICC -c $OPT -Wall -I$SHARED -fPIC gateway.c

ar -cr ../../lib/libempi-backend.a \
logical.o \
debugging.o \
message_queue.o \
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
gateway.o \
empi.o

#wa_sockets.o \

rm -f *.o
