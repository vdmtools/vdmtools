

################## Tools to be used ##################
LATEX = latex
PLATEX = platex
PDFLATEX = altjpdflatex
#PDFLATEX = pdflatex
DVIPS = dvips 
BIBTEX = pbibtex
MAKEINDEX = makeindex
MENDEX = mendex
A5BOOK = a5book
DVIPDFMX = dvipdfmx

OUTPUTDOCS = langmansl_a4 langmanpp_a4 langmansl_letter langmanpp_letter \
             langmanvice_a4 langmanvice_letter
SUFF = ps aux dvi log bbl blg toc idx ilg ind tex out

################## Options ##################
DVIPSOPT_A4 = 
DVIPSOPT_LETTER =-tletter
PDFOFFSET=-O0cm,1.5cm


################## Variables for dependencies ##################
INPUTFILES = sem-comtypes.tex sem-functypes.tex sem-maptypes.tex \
             sem-numtypes.tex sem-seqtypes.tex sem-settypes.tex

################## These files are automatically created. ##################
FIGFILES = 2periodic.fig vm1a.fig vm1b.fig vm1c.fig vm1d.fig vm2.fig \
           history.fig T1.fig T2.fig T3.fig
MAKEEPS = $(FIGFILES:.fig=.eps)
MAKEPDF = $(MAKEEPS:.eps=.pdf)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

OUTPUTSUFF = ps pdf

OUTPUTTARGETS = $(foreach suf, $(OUTPUTSUFF), $(foreach b, $(OUTPUTDOCS),$(b).$(suf)))

################## Rules ##################

makeeps: $(MAKEEPS)

makepdf: $(MAKEPDF)

%E.pdf: %E.tex $(INPUTFILES) history.eps titlelogo.png ifad.bib
	  rm -f $*.aux $*.out $*.toc $@
	  $(PLATEX) $<
	  $(BIBTEX) $(<:E.tex=E)
	  $(MAKEINDEX) $*E.idx
	  $(PLATEX) $<
	  $(PLATEX) $<
	  $(DVIPDFMX) $(<:E.tex=E.dvi)

#%.pdf: %.tex $(INPUTFILES) makepdf titlelogo.png ifad.bib
#	  rm -f $*.aux $*.out $*.toc
#	  $(PDFLATEX) $<
#	  $(BIBTEX) $(<:.tex=)
#	  $(MAKEINDEX) $*.idx
#	  $(PDFLATEX) $<
#	  $(PDFLATEX) $<

my%.pdf: langman%_letter.tex $(INPUTFILES) $(MAKEPDF) titlelogo.png ifad.bib
	  rm -f $*.aux $*.out $*.toc
	  $(PDFLATEX) $<

lang%.dvi: lang%.tex $(INPUTFILES) makeeps ifad.bib
	  $(LATEX) $<
	  $(BIBTEX) $(<:.tex=)
	  $(MAKEINDEX) lang$*.idx
	  $(LATEX) $<
	  $(LATEX) $<

%_a4.ps: %_a4.dvi
	  $(DVIPS) $(DVIPSOPT_A4) $< -o $@

%_letter.ps: %_letter.dvi
	  $(DVIPS) $(DVIPSOPT_LETTER) $< -o $@

%_book.ps: %_a4.ps
	  $(A5BOOK) < $< > $@

%.eps: %.fig
	fig2dev -L eps $< $@

%.pdf: %.eps
	epstopdf --debug $<


################## Targets ##################

#all: langmansl_a4E.pdf langmanpp_a4E.pdf langmanvice_a4E.pdf #bnf_sl.html bnf_pp.html
all: langmansl_a4E.pdf langmanpp_a4E.pdf

alltargets:	$(OUTPUTTARGETS)

binders: langmansl_book.ps langmanpp_book.ps langmanvice_book.ps

langmansl_a4E.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVDMSL -DA4Format $< > $@
	chmod a-w $@

langmansl_letter.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVDMSL -DLetterFormat $< > $@
	chmod a-w $@

langmanpp_a4E.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVDMPP -DA4Format $< > $@
	chmod a-w $@

langmanpp_letter.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVDMPP -DLetterFormat $< > $@
	chmod a-w $@

langmanvice_a4E.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVICEMAN -DVDMPP -DA4Format $< > $@
	chmod a-w $@

langmanvice_letter.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVICEMAN -DVDMPP -DLetterFormat $< > $@
	chmod a-w $@

ifad.bib: ../bib/ifad.bib
	cp $< $@

#ifad.bib: ../bib/ifad.bib
#	ln -s $< $@

bnf_sl.html: langmansl_a4E.tex
	./convert.pl $< $@

bnf_pp.html: langmanpp_a4E.tex
	./convert.pl $< $@

bnf_vice.html: langmanvice_a4E.tex
	./convert.pl $< $@


# What to clean. 
clean: 
	rm -f $(TEXTMP) bnf_sl.html bnf_pp.html $(MAKEEPS) $(MAKEPDF)
	rm -f langmansl_book.ps langmanpp_book.ps langmanvice_book.ps ifad.bib
	rm -f *.aux *.bbl *.blg *.dvi *.idx *.ilg *.ind *.log *.out *.toc *E.tex
