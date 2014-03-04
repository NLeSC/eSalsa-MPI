#!/bin/bash

CC=gcc

rm -f *.o
rm -f ../../lib/libempi-logging.a

$CC -c -g -Wall -fPIC logging.c
$CC -c -g -Wall -fPIC profiling.c

ar -cr ../../lib/libempi-logging.a \
logging.o \
profiling.o

rm -f *.o

