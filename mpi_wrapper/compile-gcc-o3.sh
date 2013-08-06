#!/bin/bash

module purge
module list

MPI=/var/scratch/jason/OpenMPI/openmpi-1.4.2-fixed-gnu
MPICC=$MPI/bin/mpicc

echo using `which $MPICC`

rm -f *.o ../libmpibis.a

gfortran -c logical.f

$MPICC -c -O3 -Wall logging.c
$MPICC -c -O3 -Wall generated_mpi.c
$MPICC -c -O3 -Wall debugging.c
#$MPICC -c -O3 -Wall generated_impi.c
$MPICC -c -O3 -Wall wa_sockets.c
$MPICC -c -O3 -Wall request.c
$MPICC -c -O3 -Wall communicator.c
$MPICC -c -O3 -Wall messaging.c
$MPICC -c -O3 -Wall group.c
$MPICC -c -O3 -Wall mpibis.c
$MPICC -c -O3 -Wall operation.c
$MPICC -c -O3 -Wall reduce_ops.c
$MPICC -c -O3 -Wall profiling.c

ar -cr ../libmpibis.a logging.o generated_mpi.o debugging.o mpibis.o request.o communicator.o messaging.o wa_sockets.o group.o operation.o reduce_ops.o logical.o profiling.o
#ar -cr ../libmpibis.a generated_mpi.o debugging.o generated_impi.o

rm -f *.o
