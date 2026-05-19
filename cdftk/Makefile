include Makefile.inc

all: dist

.PHONY: build
.PHONY: test
.PHONY: clean-build

build:
	make -C src

test: build
	make -C test test

valgrind: build
	make -C test valgrind

clean-build: build
	src/build/cdftk clean

clean:
	rm -rf dist
	make -C src clean
	make -C test clean


dist: build
	mkdir -p dist/bin
	cp $(BUILD_DIR)/cdftk dist/bin

run: build
	make -C src run

install: dist
	$(info Installing to $(MOD_INSTALL_PATH))
	rm -rf $(MOD_INSTALL_PATH)
	mkdir -p $(MOD_INSTALL_PATH)
	cp -r dist/* $(MOD_INSTALL_PATH)
	cp cdfmodule.json $(MOD_INSTALL_PATH)
