include Makefile.inc

SUBDIRS := $(patsubst %/.,%,${wildcard */.})
SUBDIRS := $(foreach dir, $(SUBDIRS), $(if $(wildcard $(dir)/Makefile), $(dir)))

all:
	$(foreach subdir, $(SUBDIRS), make -C $(subdir); )

clean:
	$(foreach subdir, $(SUBDIRS), make -C $(subdir) clean; )

