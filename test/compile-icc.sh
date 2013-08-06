#!/bin/bash

module purge
module add intel/compiler/64/12.0/2011.5.220
module add openmpi/intel/64/1.4.2 
module add netcdf/intel/64/4.1.1
module list 

rm -f *.o app.exe
icc -Wall app5.c -o app.exe -I/cm/shared/apps/openmpi/intel/64/1.4.2/include -L.. -lmpibis -L/cm/shared/apps/openmpi/intel/64/1.4.2/lib64 -lmpi -limf -lm
