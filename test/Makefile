include ../Makefile.inc

TESTS = test_allreduce.exe test_border.exe test_chrongear.exe test_collectives.exe test_group.exe test_init.exe test_irecv_self.exe test_isend_irecv.exe test_send_irecv.exe test_send_recv.exe test_split.exe test-throughput.exe

LIBS = -L$(LIBDIR) -lempi-frontend -lempi-backend -lempi-shared -lmpi 

all: $(TESTS)
	$(AR) -cr $(OBJLIB) $(OBJECTS)

%.exe : %.c
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@ $(LIBS)

clean:
	$(RM) -f $(OBJLIB) $(TESTS)




