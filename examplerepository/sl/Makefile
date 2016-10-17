# Master Makefile for examples directory. 

SUBDIRS = acs adt bar boiler cbs crossword dfdexample express graph-ed library \
	  metro msmie loose maa ndb newspeak planner proglang raildir realm \
	  simulator soccer stv trigno
# advanced should be added again when new DL is available
# minic should be added again when syntax and type errors have been corrected.

.PHONY: $(SUBDIRS) generate clean

all: generate all.zip

clean: 
	for i in $(SUBDIRS); do \
          $(MAKE) -C $$i -f Makefile clean; \
        done
	rm *.tar.gz *.ps.gz *.vdm.gz all.zip

generate: 
	for i in $(SUBDIRS); do \
          $(MAKE) -C $$i -f Makefile all installupdir || exit 1; \
        done

# This is so one can write e.g., 'make acs' if only one dir is wanted
$(SUBDIRS): 
	$(MAKE) -C $@ -f Makefile all installupdir

all.zip:
	zip -l all.zip */*.vdm */*.avdm

install: all
	install -m 444 *.gz skeleton.tex all.zip README /home/ftp/pub/vdm/examples

html: examples.tex
	latex2html -init_file .latex2html-init examples.tex

