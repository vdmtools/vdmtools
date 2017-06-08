# $Id: Makefile,v 1.46 2005/12/15 04:56:24 vdmtools Exp $

LATEX = latex
PLATEX = platex
PDFLATEX = pdflatex
DVIPS = dvips 
DVIPDFM = dvipdfm
BIBTEX = bibtex
MAKEINDEX = makeindex
A5BOOK = a5book

OUTPUTDOCS = integrityman
SUFF = ps aux dvi log bbl blg toc idx ilg ind pdf out

DVIPSOPT = 
PDFOFFSET=-O0cm,1.5cm


%.eps: %.fig
	fig2dev -L eps $< $@

PDF_FROM_EPS =  $(EPSFROMFIG:.eps=.pdf)
EPS_FROM_PNG = $(PNGFILES:.png=.eps)
JPG_FROM_PNG = $(PNGFILES:.png=.jpg)

PDFFILES = $(PDF_FROM_EPS) 

A4FLAG = $(DVIPSOPT_$(patsubset %.%,A4,$@))
DVIPSOPTS = $(A4LETTERFLAG)

%.ps:	%.dvi
	$(DVIPS) $(DVIPSOPTS) $< -o $@

$(PDF_FROM_EPS): %.pdf: %.eps
	epstopdf $<

$(EPS_FROM_PNG): %.eps: %.png
	convert $< $@

$(JPG_FROM_PNG): %.jpg: %.png
	convert $< $@

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

#all: integritymansl.pdf integritymanpp.pdf 
all: integritymansl_a4E.pdf integritymanpp_a4E.pdf 

binders: $(BINDERS)

jpegs: $(JPG_FROM_PNG)

integritymansl.ps: integritymansl.dvi
	$(DVIPS) $(DVIPSOPT) $< -o $@

integritymansl_book.ps: integritymansl.ps
	$(A5BOOK) < $< > $@

integritymanpp.ps: integritymanpp.dvi
	$(DVIPS) $(DVIPSOPT) $< -o $@

integritymanpp_book.ps: integritymanpp.ps
	$(A5BOOK) < $< > $@

%_book.ps: %.ps
	$(A5BOOK) < $< > $@

integritymansl.pdf: integritymansl.tex  dan.bib $(PDFFILES) $(EPSFROMFIG)
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

integritymansl_a4E.pdf: integritymansl_a4E.tex  dan.bib $(PDFFILES) $(EPSFROMFIG)
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFM) $(<:.tex=)

integitymansl.pdf: integritymansl.pdf
	cp -f $< $@
	chmod a-w $@

integritymanpp.dvi: integritymanpp.tex  dan.bib $(EPS_FROM_PNG) $(EPSFROMFIG)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

integritymanpp.pdf: integritymanpp.tex dan.bib $(PDFFILES) $(EPSFROMFIG)
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

integritymanpp_a4E.pdf: integritymanpp_a4E.tex dan.bib $(PDFFILES) $(EPSFROMFIG)
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFM) $(<:.tex=)

integritymanvice.pdf: integritymanvice.tex dan.bib $(PDFFILES) $(EPSFROMFIG)
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

integritymansl.tex: integrityman.tex 
	rm -f $@
	ifdeffilt -DVDMSL $< > $@
	chmod a-w $@

integritymansl_a4E.tex: integrityman.tex 
	rm -f $@
	ifdeffilt -DVDMSL $< > $@
	chmod a-w $@

integritymanpp.tex: integrityman.tex 
	rm -f $@
	ifdeffilt -DVDMPP $< > $@
	chmod a-w $@

integritymanpp_a4E.tex: integrityman.tex 
	rm -f $@
	ifdeffilt -DVDMPP $< > $@
	chmod a-w $@

integritymanvice.tex: integrityman.tex
	rm -f $@
	ifdeffilt -DVICEMAN -DVDMPP $< > $@
	chmod a-w $@

# What to clean. 
clean: 
	rm -f $(TEXTMP) $(PDFFILES) $(EPSFILES)  $(EPSFROMFIG)\
              fig-syn-err.eps \
              integritymanpp.tex integritymansl.tex \
              integritymanpp_a4E.tex integritymansl_a4E.tex \
							*.aux *.log *.bbl *.ilg *.idx *.toc *.out *.dvi *.blg *.ind \
	      $(BINDERS)

