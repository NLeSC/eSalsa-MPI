include ../../empi.config

OBJECTS = socket_util.o linked_queue.o blocking_linked_queue.o logical.o communicator.o debugging.o file.o group.o message_queue.o data_message_queue.o operation.o request.o datatype.o empi.o info.o messaging.o reduce_ops.o status.o udt4.o udt_util.o gateway.o 

LIBDIR  = $(EMPI_HOME)/lib

SHARED  = -I$(EMPI_HOME)/src/shared
OBJLIB  = $(LIBDIR)/libempi-backend.a
UDT     = -I$(UDT_HOME)/src

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(MPICC) -c -Wall $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $(SHARED) $< -o $@

.cpp.o: 
	$(MPICC) -c -Wall $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $(UDT) $(SHARED) $< -o $@

.f.o:
	$(FC) -c $(OPT) $(DEBUG) $(PROF) $(FFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




