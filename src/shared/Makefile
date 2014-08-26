include ../../empi.config

OBJECTS = logging.o profiling.o 

LIBDIR  = $(EMPI_HOME)/lib

OBJLIB  = $(LIBDIR)/libempi-shared.a

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(CC) -c $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




