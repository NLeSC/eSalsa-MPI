DIRS = src/shared src/frontend src/backend src/gateway src/test 

libs: check-env
	mkdir -p lib
	for d in $(DIRS); do (cd $$d; $(MAKE)); done

check-env:
ifndef EMPI_HOME
   $(error EMPI_HOME is undefined)
endif

clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done        
	rm -rf lib

server:
	cd src/server ; ant

clean-server:
	cd src/server ; ant clean

all: libs server

distclean: clean clean-server


	
