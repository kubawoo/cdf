include Makefile.inc
ALL_OBJECTS := $(foreach m, $(MODULES), $(m)/src/*.o)

all: dist

.PHONY: build
.PHONY: test

build:
	$(info Building the following modules: $(MODULES)) 
	$(foreach m, $(MODULES), make -C $(m); )

test: build
	$(foreach m, $(MODULES), make -C $(m) test && ) echo 'All tests passed!'

valgrind: build
	$(foreach m, $(MODULES), make -C $(m) valgrind && ) echo 'All tests passed'

clean:
	rm -rf dist
	$(foreach m, $(MODULES), make -C $(m) clean; )

dist: build
	mkdir -p dist/lib
	mkdir -p dist/include
	$(foreach m, $(MODULES), cp $(m)/src/*.h dist/include; )
	$(foreach m, $(MODULES), echo '#include "$(m).h"' >> dist/include/cdf.h; )
	$(CC) $(LDFLAGS) -shared $(ALL_OBJECTS) -o dist/lib/libcdf.so

install: dist
	$(info Installing to $(MOD_INSTALL_PATH))
	rm -rf $(MOD_INSTALL_PATH)
	mkdir -p $(MOD_INSTALL_PATH)
	cp -r dist/* $(MOD_INSTALL_PATH)
	cp cdfmodule.json $(MOD_INSTALL_PATH)
