
LATEX = platex
BIBTEX = pbibtex
DVIPS = dvips 
MAKEINDEX = mendex
DVIPDFMX = dvipdfmx

OUTPUTDOCS = manager
SUFF = ps aux dvi log toc idx bbl blg ind ilg

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

%.eps: %.fig
	fig2dev -L ps -z A4 $< $@

# OMTEPS files are generated from within 'oedit' via the VDM++ to OMT generator
OMTEPS = basetoolsinterface.eps     interfaceinterface.eps     toolcollinherit.eps\
	 repdatabasedetailview.eps  toolstotalview.eps\
	 error.eps                  repositoryasso.eps         updateprojinterface.eps\
	 errorsinterface.eps        repositoryinterface.eps    updatesesinterface.eps\
	 errorstate.eps             statusinfointerface.eps    dependinterface.eps

# FIGEPS files are generated from 'xfig' .fig files. 
FIGEPS = usecase.eps contextoverview.eps

# VDM++ specification files
VPPSRC = $(shell ls  ../*.vpp)
VPPTEX = $(VPPSRC:%.vpp=%.vpp.tex)

# Handcoded files included in documentation:
PRELIM = ../code/*.cc

%.vpp.tex: %.vpp
	vppde -lN $<

all: manager.pdf

manager.pdf: manager.dvi
	$(DVIPDFMX) $<

#manager.dvi: manager.tex $(FIGEPS) $(VPPTEX) $(OMTEPS) $(PRELIM)
manager.dvi: manager.tex $(FIGEPS) $(VPPTEX) $(OMTEPS)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) $(<:.tex=)
	$(LATEX) $<
	$(LATEX) $<


# What to clean. 
clean: 
	rm -f $(TEXTMP)  $(FIGEPS) *.vpp.tex

