include ../../Makefile.inc

OBJECTS = empif_wrapper.o empi_wrapper.o
OBJLIB = $(LIBDIR)/libempi-frontend.a

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $(SHARED) $(BACKEND) $< -o $@

.f.o:
	$(FC) -c $(FFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




