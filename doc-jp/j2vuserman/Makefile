PLATEX = platex
DVIPDFMX = dvipdfmx
BIBTEX = bibtex
EBB = ebb

OUTPUTDOCS = j2vuserman_a4J
SUFF = ps aux dvi log toc idx blg bbl tex out

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

FIGDIR = figures

PNGFILES = plus.png minus.png syntaxcheckmodified.png \
        syntaxcheck.png syntaxcheckdone.png syntaxcheckerror.png \
        typecheck.png typecheckerror.png typecheckdone.png \
        java2vdm.png java2vdmdone.png java2vdmerror.png \
        addJavaFiles.png addedJavaFiles.png javaView.png \
        javaLibFiles.png vdmFiles.png vdmApiFiles.png \
        translationDone.png j2vOptions.png projectoptions.png 

#EPS_FROM_PNG = $(PNGFILES:.png=.eps)

#EPSFILES = $(EPS_FROM_PNG)

%.png:
	cd $(FIGDIR); $(EBB) $@

EPS_FROM_PNG = $(PNGFILES)

#$(EPS_FROM_PNG): %.eps: %.png
#	convert $< $@

all:  j2vuserman_a4J.pdf
#all:  j2vuserman_a4.ps  j2vuserman_letter.ps \
#      j2vuserman_a4.pdf j2vuserman_letter.pdf

#
# Java to VDM++ Translator Manual
#
j2vuserman_a4J.pdf: j2vuserman_a4J.tex ifad.bib $(EPS_FROM_PNG)
	rm -f $@
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

j2vuserman_a4J.tex: j2vuserman.tex
	rm -f $@
	ifdeffilt -DVDMPP -DA4Format $< > $@
	chmod a-w $@

ifad.bib:
	cp ../bib/ifad.bib .

# What to clean. 
clean: 
	rm -f $(TEXTMP) j2vuserman_book.ps $(EPSFILES) ifad.bib
	rm -f $(FIGDIR)/*.bb
