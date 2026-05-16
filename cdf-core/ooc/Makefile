all: build

.PHONY: build
.PHONY: test

build:
	make -C src

test: build
	make -C test test

valgrind: build
	make -C test valgrind

clean:
	make -C src clean
	make -C test clean

