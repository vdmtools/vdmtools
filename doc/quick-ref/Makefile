
LATEX = latex
PLATEX = platex
DVIPS = dvips 
DVIPDFMX = dvipdfmx

OUTPUTDOCS = QuickRef
SUFF = ps aux dvi log bbl blg toc


GENERATED = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 


all: QuickRef.pdf
#all: QuickRef.ps

QuickRef.ps: QuickRef.dvi 
	$(DVIPS) $< -o $@

QuickRef.dvi: QuickRef.tex 
	$(LATEX) $<
	$(LATEX) $<

QuickRef.pdf: QuickRef.tex
	$(PLATEX) $<
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

# The next two targets are only used by ~vdm/release/Makefile which 
# sets DVIPSOPT before making these targets. 

QuickRef_a4.ps: QuickRef.dvi 
	rm -f $@
	$(DVIPS) $(DVIPSOPT) $< -o $@

QuickRef_letter.ps: QuickRef.dvi
	rm -f $@
	$(DVIPS) $(DVIPSOPT) $< -o $@

clean: 
	rm -f $(GENERATED) \
	    QuickRef_a4.ps QuickRef_letter.ps QuickRef.pdf QuickRef.idx
