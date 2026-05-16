include Makefile.inc

SUBDIRS := ${wildcard */.}

all:
	$(foreach subdir, $(SUBDIRS), make -C $(subdir); )

clean:
	$(foreach subdir, $(SUBDIRS), make -C $(subdir) clean; )

