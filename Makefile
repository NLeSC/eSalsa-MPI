DIRS = src/shared src/frontend src/backend test

all:
	for d in $(DIRS); do (cd $$d; $(MAKE)); done

clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done        



	
