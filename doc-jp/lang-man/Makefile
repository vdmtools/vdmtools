######################################################
#                                                    #
#         VDMTools Language manual Makefile          #
#                                                    #
######################################################

################## Tools to be used ##################
LATEX = latex
PLATEX = platex -kanji=utf8
BIBTEX = pbibtex
#BIBTEX = jbibtex
MAKEINDEX = makeindex
MENDEX = mendex -U
DVIPDFMX = dvipdfmx

OUTPUTDOCS = langmansl_a4J langmanpp_a4J langmanvice_a4J
SUFF = aux dvi log bbl blg toc idx ilg ind tex out

MAKEEPS = $(FIGFILES:.fig=.eps)
TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

################## Rules ##################

%J.pdf: %J.tex history.eps ifad.bib
	  rm -f $*.aux $*.out $*.toc $@
	  $(PLATEX) $<
	  $(BIBTEX) $(<:J.tex=J)
	  $(MAKEINDEX) $*J.idx
	  $(PLATEX) $<
	  $(PLATEX) $<
	  $(DVIPDFMX) $(<:J.tex=J.dvi)

%.eps: %.fig
	fig2dev -L eps $< $@

################## Targets ##################

all: langmansl_a4J.pdf langmanpp_a4J.pdf
#all: langmansl_a4J.pdf langmanpp_a4J.pdf langmanvice_a4J.pdf

langmansl_a4J.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVDMSL -DA4Format $< > $@
	chmod a-w $@

langmanpp_a4J.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVDMPP -DA4Format $< > $@
	chmod a-w $@

langmanvice_a4J.tex: lang_man.tex 
	rm -f $@
	ifdeffilt -DVICEMAN -DVDMPP -DA4Format $< > $@
	chmod a-w $@

ifad.bib: ../../doc/bib/ifad.bib
	nkf -e -Lu -d $< > $@

# What to clean. 
clean: 
	rm -f $(TEXTMP) $(MAKEEPS) ifad.bib
