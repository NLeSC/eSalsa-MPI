include $(EMPI_HOME)/empi.config

TESTS =  test_core.exe test_dup.exe test_irecv_self2.exe test-throughput-TCP.exe test-throughput-TCP1.exe test-throughput-TCP2.exe test_allreduce.exe test_collectives.exe test_group.exe test_init.exe test_irecv_self.exe test_isend_irecv.exe test_send_irecv.exe test_send_recv.exe test_split.exe test-throughput.exe test-throughput2.exe test-throughput-OCN.exe test-throughput-ATM.exe test_barrier.exe test-latency.exe test_scan_world.exe

all: $(TESTS)

.c.o:
	$(EMPI_HOME)/scripts/empicc $(OPT) $(DEBUG) $(PROF) $(CFLAGS) -c $< -o $@

%.exe : %.o
	$(EMPI_HOME)/scripts/empild $(PROF) $< -o $@ 

clean:
	$(RM) -f $(OBJLIB) $(TESTS)




