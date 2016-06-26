all:
	$(MAKE) -C ./src/minimal-boostlambda all
	$(MAKE) -C ./src/mylambda all

clean:
	$(MAKE) -C ./src/minimal-boostlambda clean
	$(MAKE) -C ./src/mylambda clean

