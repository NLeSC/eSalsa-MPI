DIRS = src/shared src/frontend src/backend

all:
	for d in $(DIRS); do (cd $$d; $(MAKE)); done

clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done        



	
