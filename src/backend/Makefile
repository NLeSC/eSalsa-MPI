include ../../empi.config

OBJECTS = linked_queue.o logical.o communicator.o debugging.o file.o group.o message_queue.o data_message_queue.o operation.o request.o datatype.o empi.o info.o messaging.o reduce_ops.o status.o gateway.o

LIBDIR  = $(EMPI_HOME)/lib

SHARED  = -I$(EMPI_HOME)/src/shared
OBJLIB  = $(LIBDIR)/libempi-backend.a

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(MPICC) -c -Wall $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $(SHARED) $< -o $@

.f.o:
	$(FC) -c $(OPT) $(DEBUG) $(PROF) $(FFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




