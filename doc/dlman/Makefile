PLATEX = platex
DVIPDFMX = dvipdfmx
BIBTEX = bibtex
A5BOOK = a5book

OUTPUTDOCS = dlmansl_a4E
SUFF = ps aux dvi log toc idx blg bbl tex out

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) \
         approach.eps approach.pdf
#TRIGNOFILES = trigno/Makefile.Linux trigno/Makefile.solaris2.6 trigno/Makefile.win32 \
#              trigno/cylinder.vdm trigno/cylio.vdm trigno/math.vdm \
#              trigno/cylio.h trigno/cylio.cc trigno/tcfcylio.cc trigno/tcfmath.cc 

%.eps: %.fig
	fig2dev -L eps $< $@

%.pdf: %.eps
	epstopdf --debug $<

%.eps: %.png
	convert $< $@	

EPSFROMFIG = approach.eps
PDFFROMEPS = approach.pdf


all: dlmansl_a4E.pdf

dlmansl_a4E.pdf: dlmansl_a4E.tex ifad.bib $(EPSFROMFIG) cptrigno # $(TRIGNOFILES)
	rm -f dlmansl_a4.aux dlmansl_a4.out dlmansl_a4.toc $@
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

dlmansl_a4E.tex: DLMan.tex
	rm -f $@
	ifdeffilt -DVDMSL -DA4Format $< > $@
	chmod a-w $@

ifad.bib: ../bib/ifad.bib
	cp $< $@

cptrigno:
	cp -r ../trigno_example trigno

# What to clean. 
clean: 
	rm -f $(TEXTMP) $(PDFFROMEPS) $(EPSFROMFIG) ifad.bib
	rm -rf trigno
#	cd trigno; $(MAKE) -f Makefile.doc clean
#	cd trigno; $(MAKE) -f Makefile.Linux clean
