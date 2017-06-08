LATEX = latex
PLATEX = platex
#PDFLATEX = pdflatex
DVIPDFMX = dvipdfmx
DVIPDFM = dvipdfm
#DVIPS = dvips 
BIBTEX = bibtex
JBIBTEX = jbibtex
MAKEINDEX = makeindex
#A5BOOK = a5book
EBB = ebb

OUTPUTDOCS = ApiMan_a4E
SUFF = ps aux dvi log bbl blg toc idx ilg ind

#DVIPSOPT_A4 = 
#DVIPSOPT_LETTER = -x950 -tletter
#PDFOFFSET=-O0cm,1.5cm


#%.eps: %.fig
#	fig2dev -L ps -z A4 $< $@

#%.pdf: %.eps
#	epstopdf --debug $<

#%.eps: %.png
#	convert $< $@	

#PNG = VDMApplication.png VDMParser.png VDMInterpreter.png VDMErrors.png VDMvalues.png VDMModuleRepos.png VDMProject.png 

#EPSFILES = VDMApplication.eps VDMParser.eps VDMInterpreter.eps VDMErrors.eps VDMvalues.eps VDMModuleRepos.eps VDMProject.eps VDMApplication.eps

# These files are automatically created. 
#MAKEPDF = $(EPSFILES:.eps=.pdf)

# These files are automatically created. 
#EPSFROMPNG = $(patsubst %.png, %.eps, $(PNG))


TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

#all: ApiMan_a4.ps ApiMan_letter.ps ApiMan_a4.pdf ApiMan_letter.pdf 
all: ApiMan_a4E.pdf

#binders: ApiMan_book.ps

#ApiMan_a4.tex: api_man.tex example/client_example.java example/client_example.cc
#	rm -f $@
#	ifdeffilt -DA4Format $< > $@
#	chmod a-w $@

ApiMan_a4E.tex: api_man.tex example/client_example.java example/client_example.cc
	rm -f $@
	ifdeffilt -DENG -DA4Format $< > $@
	chmod a-w $@

#ApiMan_letter.tex: api_man.tex example/client_example.java example/client_example.cc
#	rm -f $@
#	ifdeffilt -DLetterFormat $< > $@
#	chmod a-w $@

#ApiMan_a4.ps: ApiMan_a4.dvi
#	$(DVIPS) $(DVIPSOPT_A4) $< -o $@

#ApiMan_letter.ps: ApiMan_letter.dvi
#	$(DVIPS) $(DVIPSOPT_LETTER) $< -o $@


#ApiMan_a4.pdf: ApiMan_a4.tex $(MAKEPDF)
#ApiMan_a4.pdf: ApiMan_a4.tex
#	$(PLATEX) $<
#	$(JBIBTEX) $(<:.tex=)
#	$(PLATEX) $<
#	$(PLATEX) $<
#	$(DVIPDFMX) $(<:.tex=.dvi)

#	$(PDFLATEX) $< 
#	$(BIBTEX) $(<:.tex=)
#	$(PDFLATEX) $<
#	$(PDFLATEX) $<

# teramoto added to build japanese manual
#%.dvi: %.tex ifad.bib $(MAKEPDF)
#	$(LATEX) $<
#	$(BIBTEX) $(<:.tex=)
#	$(LATEX) $<
#	$(LATEX) $<

#ApiMan_a4E.pdf: ApiMan_a4E.tex ifad.bib $(EPSFROMPNG) 
ApiMan_a4E.pdf: ApiMan_a4E.tex ifad.bib makebb
	rm -f $(<:.tex=)E.aux $(<:.tex=)E.out $(<:.tex=).toc
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

#%E.pdf: %E.tex ifad.bib $(MAKEPDF)
#	rm -f $(<:E.tex=).aux $(<:E.tex=).out $(<:E.tex=).toc
#	$(PLATEX) $<
#	$(BIBTEX) $(<:E.tex=E)
#	$(PLATEX) $<
#	$(PLATEX) $<
#	$(DVIPDFMX) $(<:E.tex=E.dvi)
    

#ApiMan_letter.pdf: ApiMan_letter.tex $(MAKEPDF)
#	$(PDFLATEX) $< 
#	$(BIBTEX) $(<:.tex=)
#	$(PDFLATEX) $<
#	$(PDFLATEX) $<

#myApiMan.pdf: ApiMan_letter.tex $(MAKEPDF)
#	$(PDFLATEX) $< 

#ApiMan_a4.dvi: ApiMan_a4.tex $(EPSFILES)
#	$(LATEX) $<
#	$(BIBTEX) $(<:.tex=)
#	$(LATEX) $<
#	$(LATEX) $<

#ApiMan_letter.dvi: ApiMan_letter.tex $(EPSFILES)
#	$(LATEX) $<
#	$(BIBTEX) $(<:.tex=)
#	$(LATEX) $<
#	$(LATEX) $<

#ApiMan_book.ps: ApiMan_a4.ps
#	$(A5BOOK) < $< > $@

ifad.bib: ../bib/ifad.bib
	cp $< $@

makebb:
	$(EBB) *.png


# What to clean. 
clean: 
	rm -f $(TEXTMP) ifad.bib 
	rm -f $(TEXTMP) ApiMan_a4.ps ApiMan_letter.ps 
	rm -f ApiMan_a4.pdf ApiMan_letter.pdf ApiMan_a4.tex ApiMan_letter.tex ApiMan_book.ps $(MAKEPDF) *.out
	rm -f *.aux *.log *.idx *.toc *.bbl *.blg *.dvi ApiMan_a4E.tex *.eps *.bb

realclean: clean
	rm -f ApiMan_a4E.pdf
