include ../../Makefile.inc

OBJECTS = logging.o profiling.o 
OBJLIB = $(LIBDIR)/libempi-shared.a

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




