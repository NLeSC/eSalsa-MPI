module load java/oracle 
module load gcc/4.8.0 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SURFSARA_IMPI_LIB
#export LD_LIBRARY_PATH=/home/jason/udt4/src:$LD_LIBRARY_PATH
export EMPI_HOME=/home/jason/eSalsa-MPI-MT
export EMPI_GATEWAY_CONFIG=$EMPI_HOME/gateway.cart.config 
export EMPI_APPLICATION_CONFIG=$EMPI_HOME/app.cart.config 
