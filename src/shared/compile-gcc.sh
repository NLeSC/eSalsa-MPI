#!/bin/bash

CC=gcc

OPT=-O3

rm -f *.o
rm -f ../../lib/libempi-logging.a

$CC -c $OPT -Wall -fPIC logging.c
$CC -c $OPT -Wall -fPIC profiling.c

ar -cr ../../lib/libempi-logging.a \
logging.o \
profiling.o

# gcc logging.o profiling.o -shared -o ../../lib/libmpi-logging.so

rm -f *.o

