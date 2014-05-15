# put common definitions in here
EMPI_HOME = /home/jason/Workspace/eSalsa-MPI
CC  	= gcc
FC      = gfortran
MPICC   = mpicc
CFLAGS	= -O3 -Wall
AR	= ar
ARFLAGS	= 
RANLIB	= ranlib
RM	= rm
ECHO	= echo
INCLUDE = -I$(EMPI_HOME)/include 
SHARED  = -I$(EMPI_HOME)/src/shared 
BACKEND = -I$(EMPI_HOME)/src/backend
LIBDIR  = $(EMPI_HOME)/lib
SHELL	= /bin/sh

