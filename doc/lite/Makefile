LATEX = latex
BIBTEX = bibtex
DVIPS = dvips 

OUTPUTDOCS = lite
SUFF = ps aux dvi log toc idx bbl blg

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

all: lite.ps

lite.ps: lite.dvi 
	$(DVIPS) $< -o $@

lite.dvi: lite.tex
	$(LATEX) $<
	$(LATEX) $<


# What to clean. 
clean: 
	rm -f $(TEXTMP)  

