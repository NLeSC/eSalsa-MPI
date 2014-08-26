include ../../empi.config

OBJECTS = empif_wrapper.o empi_wrapper.o

LIBDIR  = $(EMPI_HOME)/lib

OBJLIB  = $(LIBDIR)/libempi-frontend.a

INCLUDE = -I$(EMPI_HOME)/include
SHARED  = -I$(EMPI_HOME)/src/shared
BACKEND = -I$(EMPI_HOME)/src/backend

all: $(OBJECTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

.c.o:
	$(CC) -c $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $(INCLUDE) $(SHARED) $(BACKEND) $< -o $@

.f.o:
	$(FC) -c $(OPT) $(DEBUG) $(PROF) $(FFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS)




