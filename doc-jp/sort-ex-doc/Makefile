PLATEX  = platex
DVIPDFMX = dvipdfmx
DVIPS  = dvips 

VDMDE = /usr/local/bin/vdmde

DVIPSOPT_A4 = 
DVIPSOPT_LETTER =-x950 -tletter

CONV_A4TOLETTER = conv_a4toletter.pl
REMOVETESTCOV = removetestcov.pl
INSERTVDMTOOLS = insertvdmtools.pl

all: sortsl_a4J.pdf

sort.vdm:
	rm -f $@
	perl $(INSERTVDMTOOLS) < sort-ex/sort.vdm > $@.tmp
	perl $(REMOVETESTCOV) < $@.tmp > $@
	chmod a-w $@

sort.vdm.tex: sort.vdm
	perl replace.pl $<

sort.vdm.pdf: sort.vdm.tex
	$(PLATEX) $<
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

sortsl_a4J.pdf: sort.vdm.pdf
	rm -f sortsl_a4J.pdf
	cp -f $< $@

sort_a4.ps: sort_a4.vdm.ps
	cp -f $< $@

sortdoc:
	cd sort-ex; make sort.vdm.ps

clean: 
	rm -f sort.vdm sort.vdm.tex sort.vdm.dvi sort.vdm.pdf
	rm -f *.ps 
	rm -f *.aux
	rm -f *.toc
	rm -f *.log
	rm -f *.blg
	rm -f *.bbl
	rm -f *.out
	rm -f *.tmp
	rm -f sort_letter.vdm.tex
