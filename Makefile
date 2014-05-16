DIRS = src/shared src/frontend src/backend src/test

all: check-env
	for d in $(DIRS); do (cd $$d; $(MAKE)); done
	cd src/server ; ant

check-env:
ifndef EMPI_HOME
   $(error EMPI_HOME is undefined)
endif

clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done        
	cd src/server ; ant clean



	
