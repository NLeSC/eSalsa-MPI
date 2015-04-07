module unload mpi.ibm
module load mpi.intel/4.1_gcc
module load gcc/4.7
#export LD_LIBRARY_PATH=/home/hpc/pr87ji/di68tok/udt4/src:$LD_LIBRARY_PATH
export EMPI_HOME=/home/hpc/pr87ji/di68tok/eSalsa-MPI-MT
export EMPI_GATEWAY_CONFIG=$EMPI_HOME/gateway.smuc.config
export EMPI_APPLICATION_CONFIG=$EMPI_HOME/app.smuc.config
