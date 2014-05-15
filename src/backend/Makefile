include ../../Makefile.inc

OBJECTS = logical.o communicator.o debugging.o file.o group.o message_queue.o operation.o request.o datatype.o empi.o gateway.o info.o messaging.o reduce_ops.o status.o
OBJLIB = $(LIBDIR)/libempi-backend.a

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(MPICC) -c $(CFLAGS) $(SHARED) $< -o $@

.f.o:
	$(FC) -c $(FFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




