
LATEX = latex
DVIPS = dvips 
MAKEINDEX = makeindex

OUTPUTDOCS = main
SUFF = ps aux dvi log toc idx blg bbl ind ilg
TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf)))
all: main.ps

.PHONY: clean

%.dvi: %.tex
	$(LATEX) $<
	$(LATEX) $<
	$(MAKEINDEX) $*
	$(LATEX) $<
	$(LATEX) $<

%.ps: %.dvi
	$(DVIPS) $< -o $@

clean: 
	rm -f $(TEXTMP) 
