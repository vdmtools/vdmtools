LATEX = latex
PLATEX = platex
DVIPDFM = dvipdfm
BIBTEX = bibtex
MAKEINDEX = makeindex
EBB = ebb

OUTPUTDOCS = testauto
SUFF = ps aux dvi log bbl blg toc idx ilg ind out

%.png:
	$(EBB) $@

PNGFILES = basicarch.png testseqiter.png TSclassdia.png TSIclassdia2.png TSIclassdia.png uniteskarch.png

EBB_FROM_PNG = $(PNGFILES)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

all: testautoE.pdf

testautoE.tex: testauto.tex 
	rm -f $@
	ifdeffilt -DVDMSL $< > $@
	chmod a-w $@

%E.pdf: %E.tex dan.bib $(PNGFILES)
	rm -f $@
	ebb $(PNGFILES)
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFM) $(<:.tex=)

# What to clean. 
clean: 
	rm -f $(TEXTMP) \
				testautoE.tex \
				*.aux *.log *.bbl *.ilg *.idx *.toc *.out *.dvi *.blg *.ind *.bb

