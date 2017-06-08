
LATEX = pdflatex
PLATEX = platex
PDFLATEX = pdflatex
DVIPS = dvips 
DVIPDFMX = dvipdfmx
BIBTEX = bibtex
JBIBTEX = jbibtex
A5BOOK = a5book

OUTPUTDOCS = guidelines_a4E guidelines_a4J guidelines_letter 
SUFF = ps aux dvi log bbl blg toc idx ilg ind tex out

DVIPSOPT_A4 = 
DVIPSOPT_LETTER =-tletter
PDFOFFSET=-O0cm,1.5cm


#%.pdf: %.eps
#	epstopdf --debug $<

MAKEEPS = alarm.eps expert.eps firstclassdia.eps updatedalarm.eps \
          updatedplant.eps updateddiagram.eps firstscreendump.eps \
          secondscreendump.eps javagui.eps fulldiagram.eps

# These files are automatically created. 
#MAKEPDF = $(MAKEEPS:.eps=.pdf)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

#all: guidelines_letter.pdf guidelines_a4.pdf \
#     guidelines_letter.pdf guidelines_a4.pdf
all: guidelines_a4E.pdf

binders: guidelines_book.ps

guidelines_a4J.tex: guidelines.tex 
	rm -f $@
	ifdeffilt -DJPN -DA4Format $< > $@
	chmod a-w $@

guidelines_a4E.tex: guidelines.tex 
	rm -f $@
	ifdeffilt -DENG -DA4Format $< > $@
	chmod a-w $@

guidelines_letter.tex: guidelines.tex 
	rm -f $@
	ifdeffilt -DLetterFormat $< > $@
	chmod a-w $@

%_a4.ps: %_a4.dvi
	$(DVIPS) $(DVIPSOPT_A4) $< -o $@

%_letter.ps: %_letter.dvi
	$(DVIPS) $(DVIPSOPT_LETTER) $< -o $@

guidelines_book.ps: guidelines_a4.ps
	$(A5BOOK) < $< > $@

%.dvi: %.tex ifad.bib 
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(LATEX) $<
	$(LATEX) $<

%J.pdf: %J.tex ifad.bib
	rm -f $(<:J.tex=)J.aux $(<:J.tex=)J.out $(<:J.tex=J).toc
	$(PLATEX) $<
	$(JBIBTEX) $(<:J.tex=J)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:J.tex=J.dvi)

%E.pdf: %E.tex ifad.bib
	rm -f $(<:E.tex=)E.aux $(<:E.tex=)E.out $(<:E.tex=E).toc
	$(PLATEX) $<
	$(BIBTEX) $(<:E.tex=E)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:E.tex=E.dvi)

my.pdf: guidelines_letter.tex ifad.bib $(MAKEPDF)
	rm -f guidelines_letter.aux guidelines_letter.out guidelines_letter.toc
	$(PDFLATEX) $<

ifad.bib: ../bib/ifad.bib
	cp $< $@
#	ln -s $< $@

# What to clean. 
clean: 
	rm -f $(TEXTMP) $(MAKEPDF) ifad.bib 
	rm -f guidelines_book.ps guidelines_a4.ps guidelines_letter.ps
	rm -f guidelines_a4.pdf guidelines_letter.pdf
