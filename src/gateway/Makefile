include ../../empi.config

OBJECTS = gateway.o udt4.o udt_util.o

LIBDIR  = $(EMPI_HOME)/lib

SHARED  = -I$(EMPI_HOME)/src/shared 
UDT     = -I$(UDT_HOME)/src
UDTLIB  = $(UDT_HOME)/src/libudt.a

all: $(OBJECTS) 
	$(CC) $(OBJECTS) $(UDTLIB) -L$(LIBDIR) -lempi-shared -lstdc++ -lpthread -lm -o gateway.exe

.c.o:
	$(CC) -c -Wall $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $(SHARED) $< -o $@

.cpp.o: 
	$(CC) -c -Wall $(OPT) $(DEBUG) $(PROF) $(CFLAGS) $(UDT) $(SHARED) $< -o $@

clean:
	$(RM) -f $(OBJLIB) $(OBJECTS) gateway.exe





