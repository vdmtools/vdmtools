LATEX = latex
PLATEX = platex
PDFLATEX = pdflatex
DVIPS = dvips 
DVIPDFMX = dvipdfmx
BIBTEX = bibtex 
A5BOOK = a5book

OUTPUTDOCS = libman_a4J
SUFF = ps aux dvi log bbl blg toc tex tex out

#FIGS = fig1.fig fig2.fig
FIGEPS = $(FIGS:.fig=.eps)
PDFFROMEPS = fig1.pdf fig2.pdf cgbase.pdf

GENERATED = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) \
	$(FIGEPS) $(PDFFROMEPS)

#EPSFROMFIG = fig1.eps fig2.eps

DVIPSOPT_A4 = 
DVIPSOPT_LETTER =-tletter
PDFOFFSET=-O0cm,1.5cm

%.eps: %.fig
	fig2dev -L ps $< $@

%.pdf: %.eps
	epstopdf --debug $<

%.eps: %.png
	convert $< $@	

all: libman_a4J.pdf
#all: libman_letter.pdf

binders: libman_book.ps

libman_a4.dvi: libman_a4.tex ifad.bib $(FIGEPS)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(LATEX) $<
	$(LATEX) $<

libman_letter.dvi: libman_letter.tex ifad.bib $(FIGEPS)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(LATEX) $<
	$(LATEX) $<

libman_a4J.pdf: libman_a4J.tex ifad.bib #$(EPSFROMFIG)
	rm -f $@
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

#libman_a4.pdf: libman_a4.tex ifad.bib $(PDFFROMEPS)
#	$(PDFLATEX) $<
#	$(BIBTEX) $(<:.tex=)
#	$(PDFLATEX) $<
#	$(PDFLATEX) $<

libman_letter.pdf: libman_letter.tex ifad.bib $(PDFFROMEPS)
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

libman_a4J.tex: m4lib.tex
	rm -f $@
	ifdeffilt -DA4Format $< > $@
	chmod a-w $@

libman_letter.tex: m4lib.tex
	rm -f $@
	ifdeffilt -DLetterFormat $< > $@
	chmod a-w $@

libman_a4.ps: libman_a4.dvi 
	rm -f $@
	$(DVIPS) $(DVIPSOPT_A4) $< -o $@

libman_letter.ps: libman_letter.dvi 
	rm -f $@
	$(DVIPS) $(DVIPSOPT_LETTER) $< -o $@

libman.ps: libman_a4.dvi 
	rm -f $@
	$(DVIPS) $(DVIPSOPT_A4) $< -o $@

libman_book.ps: libman_a4.ps
	$(A5BOOK) < $< > $@

ifad.bib:
	cp ../bib/ifad.bib $@


# What to clean. 

clean: 
	rm -f $(GENERATED) ifad.bib
