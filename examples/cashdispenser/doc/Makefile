TBROOT?=~/toolbox

LATEX = latex
PDFLATEX = pdflatex
DVIPS = dvips 
BIBTEX = bibtex
MAKEINDEX = makeindex
A5BOOK = a5book

OUTPUTDOCS = cashdispenser_a4 cashdispenser_letter 
SUFF = ps aux dvi log bbl blg toc idx ilg ind pdf tex out

DVIPSOPT_A4 = 
DVIPSOPT_LETTER =-tletter
PDFOFFSET=-O0cm,1.5cm


%.pdf: %.eps
	epstopdf --debug $<

MAKEEPS = channel2.eps slscreen.eps classoverview2.eps fullclassdiagram2.eps ppscreen.eps communicationview2.eps testing2.eps concurrent2.eps

LIBFILES=ifad.bib iptes.bst toolbox.sty vdmsl-2e.sty vpp.sty

# These files are automatically created. 
MAKEPDF = $(MAKEEPS:.eps=.pdf)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

#all: cashdispenser_letter.pdf cashdispenser_a4.pdf cashdispenser_letter.ps cashdispenser_a4.ps
all: cashdispenser_a4.pdf

binders: cashdispenser_book.ps

cashdispenser_a4.tex: cashdispenser.tex 
	rm -f $@
	ifdeffilt -DA4Format $< > $@
	chmod a-w $@

cashdispenser_letter.tex: cashdispenser.tex 
	rm -f $@
	ifdeffilt -DLetterFormat $< > $@
	chmod a-w $@

cashdispenser_a4.ps: cashdispenser_a4.dvi
	$(DVIPS) $(DVIPSOPT_A4) $< -o $@

cashdispenser_letter.ps: cashdispenser_letter.dvi
	$(DVIPS) $(DVIPSOPT_LETTER) $< -o $@

cashdispenser_book.ps: cashdispenser_a4.ps
	$(A5BOOK) < $< > $@

cashdispenser_a4.dvi: cashdispenser_a4.tex $(LIBFILES)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(LATEX) $<
	$(LATEX) $<

cashdispenser_letter.dvi: cashdispenser_letter.tex $(LIBFILES)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(LATEX) $<
	$(LATEX) $<

cashdispenser_a4.pdf: cashdispenser_a4.tex $(LIBFILES) $(MAKEPDF)
	rm -f cashdispenser_a4.aux cashdispenser_a4.out cashdispenser_a4.toc
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

cashdispenser_letter.pdf: cashdispenser_letter.tex $(LIBFILES) $(MAKEPDF)
	rm -f cashdispenser_letter.aux cashdispenser_letter.out cashdispenser_letter.toc
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

ifad.bib: $(TBROOT)/doc/bib/ifad.bib
	ln -s $< $@

iptes.bst: $(TBROOT)/doc/bib/iptes.bst
	ln -s $< $@

%.sty: $(TBROOT)/doc/latex/%.sty
	ln -s $< $@

# What to clean. 
clean: 
	rm -f $(TEXTMP) cashdispenser_book.ps cashdispenser_book.ps
	rm -f $(MAKEPDF) $(LIBFILES)
