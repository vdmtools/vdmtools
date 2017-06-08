PLATEX  = platex
DVIPDFMX = dvipdfmx
DVIPS  = dvips 

VPPDE = /usr/local/bin/vppde

DVIPSOPT_A4 = 
DVIPSOPT_LETTER =-x950 -tletter

CONV_A4TOLETTER = conv_a4toletter.pl
REMOVETESTCOV = removetestcov.pl
INSERTVDMTOOLS = insertvdmtools.pl

SRCDIR=sort-ex
VPPFILES = explsort.vpp implsort.vpp mergesort.vpp sorter.vpp dosort.vpp sortmachine.vpp
RTINFOFILES = dosort.vpp 

VPPTEXFILES = $(VPPFILES:%=%.tex)
#VPPTEXFILES = $(VPPFILES:%=%.tex) dosort.vpp.tex

all: sortpp_a4E.pdf
#all: sortpp_a4.pdf sortpp_letter.pdf sortpp_a4.ps sortpp_letter.ps

#inherit2.pdf 
inherit2.eps:
	-cp $(SRCDIR)/$@ .

#sort.tex: inherit2.pdf inherit2.eps $(VPPTEXFILES) sortdoc
sort.tex: inherit2.eps $(VPPTEXFILES)
	rm -f $@
	perl $(INSERTVDMTOOLS) < $(SRCDIR)/sort.tex > $@
	chmod a-w $@

$(VPPFILES):
	-cp $(SRCDIR)/$@ .

#dosort.vpp:
#	rm -f $@
#	$(REMOVETESTCOV) < $(SRCDIR)/dosort.vpp > $@
#	chmod a-w $@

$(VPPTEXFILES): %.tex: %
	perl replace.pl $<
#	$(VPPDE) -l $<.tex

sort.pdf: sort.tex inherit2.eps $(VPPTEXFILES)
	$(PLATEX) $<
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

sortpp_a4E.pdf: sort.pdf
	rm -f $@
	cp -f $< $@

sortdoc:
	cd $(SRCDIR); make sort.ps

vdm.tc:
	rm -f vdm.tc
	cd $(SRCDIR); make $@
	cp $(SRCDIR)/$@ .
#	ln -s $(SRCDIR)/$@ .

clean: 
	rm -f sort.tex sort.pdf
	rm -f sortpp_letter.tex
	rm -f *.ps 
	rm -f *.aux
	rm -f *.toc
	rm -f *.log
	rm -f *.blg
	rm -f *.bbl
	rm -f *.vpp
	rm -f *.dvi
	rm -f *.idx
	rm -f *.out
	rm -f *.vpp.tex
	rm -f *.pbm
	rm -f *.eps
#	cd $(SRCDIR); make clean
