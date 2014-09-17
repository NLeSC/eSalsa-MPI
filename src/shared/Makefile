include ../../empi.config

OBJECTS = logging.o profiling.o socket_util.o linked_queue.o sync_linked_queue.o blocking_linked_queue.o message_buffer.o

LIBDIR  = $(EMPI_HOME)/lib

OBJLIB  = $(LIBDIR)/libempi-shared.a

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(CC) -c $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




