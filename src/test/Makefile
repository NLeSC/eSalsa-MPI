include $(EMPI_HOME)/empi.config

TESTS = test_allreduce.exe test_collectives.exe test_group.exe test_init.exe test_irecv_self.exe test_isend_irecv.exe test_send_irecv.exe test_send_recv.exe test_split.exe test-throughput.exe

LIBS = -L$(EMPI_HOME)/lib -lempi-frontend -lempi-backend -lempi-shared -lmpi 

all: $(TESTS)

%.exe : %.c
	$(EMPI_HOME)/scripts/empicc $(OPT) $(CFLAGS) -I$(EMPI_HOME)/include $< -o $@ $(LIBS)

clean:
	$(RM) -f $(OBJLIB) $(TESTS)




