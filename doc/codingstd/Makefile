LATEX = pdflatex
DVIPS = dvips 
tfile=codingstd

all: $(tfile).pdf

%.ps: %.dvi
	$(DVIPS) $< -o $@

%.pdf: %.tex
	$(LATEX) $<
	$(LATEX) $<

clean: 
	rm -f *.aux *.dvi *.ps *.toc *.log

%.eps: %.fig
	fig2dev -L ps -z A4 $< $@

