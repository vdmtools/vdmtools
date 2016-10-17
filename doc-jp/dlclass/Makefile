PLATEX = platex
DVIPDFMX = dvipdfmx
BIBTEX = bibtex
MP = mpost

OUTPUTDOCS = dlmanpp_a4J
SUFF = ps aux dvi log toc idx blg bbl tex out

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

%.mps:	%.1
	cp -f $< $@

all: dlmanpp_a4J.pdf

%.pdf: %.tex overview.mps ifad.bib
	rm -f $*.aux $*.out $*.toc $@
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

%.mps:  %.1
	cp -f $< $@  

#%.1:    %.mp
#	$(MP) $*

dlmanpp_a4J.tex: dlclass_man.tex cp_dl_example
	rm -f $@
	ifdeffilt -DVDMPP -DA4Format $< > $@
	chmod a-w $@

ifad.bib:
	cp ../bib/ifad.bib .

cp_dl_example:
	cp -pr ../../examples/pp/dl-example .
	cp -p ../../code/eval/dlclass.h dl-example/.

# What to clean. 
clean: 
	rm -f $(TEXTMP) $(PDFFROMEPS) $(EPSFROMFIG)
	rm -f overview.mpx overview.log overview.mps ifad.bib
	rm -rf dl-example
