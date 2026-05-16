PROJECTS=test-framework cdf-core cdf-json cdf-http cdf-log cdf-db cdf-db-sqlite cdf-db-entity

all: build

.PHONY: build
.PHONY: test

build:
	$(info Building the following projects: $(PROJECTS)) 
	$(foreach p, $(PROJECTS), make -C $(p); )

test: build
	$(foreach p, $(PROJECTS), make -C $(p) test && ) echo 'All tests passed!'

valgrind: build
	$(foreach p, $(PROJECTS), make -C $(p) valgrind && ) echo 'All tests passed'

clean:
	$(foreach p, $(PROJECTS), make -C $(p) clean; )
	make -C examples clean
	rm -rf examples/build

install:
	$(foreach p, $(PROJECTS), make -C $(p) install; )

