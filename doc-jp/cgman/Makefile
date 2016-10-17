PLATEX = platex
DVIPDFMX = dvipdfmx
BIBTEX = bibtex
MAKEINDEX = makeindex
EBB = ebb

SORT-EXPP=sort-expp
SORT-EX=sort-ex
SORTPP_ORGDIR=../../examples/pp/sort
SORTSL_ORGDIR=../../examples/sl/sort

#VPPINPUT = ${SORT-EXPP}/dosort.vpp ${SORT-EXPP}/explsort.vpp \
#           s${SORT-EXPP}/implsort.vpp ${SORT-EXPP}/mergesort.vpp \
#           ${SORT-EXPP}/sort_pp.cc ${SORT-EXPP}/sorter.vpp \
#           ${SORT-EXPP}/sortmachine.vpp ${SORT-EXPP}/vdm_ImplSort_implicit.cc \
#           ${SORT-EXPP}/Makefile
#VSLINPUT = ${SPRT-EX}/sort_ex.cc ${SPRT-EX}/vdm_DefaultMod_implicit.cc \
#           cg.ps ${SPRT-EX}/Makefile
#COMMON   = include/cg.h include/cg_aux.h

OUTPUTDOCS = cgmansl cgmanpp cgmansl_a4 cgmanpp_a4 cgmansl_letter cgmanpp_letter
SUFF = ps aux dvi log toc idx blg bbl pdf tex out

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

FIGDIR = figures

%.png:
	cd $(FIGDIR); $(EBB) $@

DUMPS =	cgsl1.png cgsl2.png cgsl3.png cgpp1.png cgpp2.png cgpp3.png \
	cgpp4.png cgsl4.png

ICONS =	cplusplus.png

PNGFILES = $(DUMPS) $(ICONS)

PPMAKEFILES = ${SORT-EXPP}/Makefile ${SORT-EXPP}/Makefile.winnt
SLMAKEFILES =${SORT-EX}/Makefile ${SORT-EX}/Makefile.winnt

all: cgmansl_a4J.pdf cgmanpp_a4J.pdf

# VDM++ Code Generator Manual
cgmanpp_a4J.tex: cgman.tex cp_sortdir $(PPMAKEFILES)
	rm -f $@
	ifdeffilt -DVDMPP -DA4Format $< > $@
	chmod a-w $@

# VDM-SL Code Generator Manual
cgmansl_a4J.tex: cgman.tex cp_sortdir $(SLMAKEFILES)
	rm -f $@
	ifdeffilt -DVDMSL -DA4Format $< > $@
	chmod a-w $@

%J.pdf: %J.tex ifad.bib $(PNGFILES)
	rm -f $@
	$(PLATEX) $<
	$(BIBTEX) $(<:J.tex=J)
	$(MAKEINDEX) -c $(<:J.tex=J.idx)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:J.tex=J.dvi)

ifad.bib:
	cp ../bib/ifad.bib .

cp_sortdir:
	cp -r $(SORTSL_ORGDIR) $(SORT-EX)
	cp -r $(SORTPP_ORGDIR) $(SORT-EXPP)

# What to clean. 
clean: 
	rm -f $(TEXTMP) cgmansl_book.ps cgmanpp_book.ps $(FIGDIR)/*.bb  ifad.bib
	rm -rf $(SORT-EX) $(SORT-EXPP)
	rm -f *.aux *.bbl *.blg *.dvi *.idx *.ilg *.ind *.log *.out *.toc *J.tex
