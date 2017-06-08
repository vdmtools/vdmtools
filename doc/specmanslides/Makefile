
LATEX = latex
BIBTEX = bibtex
DVIPS = dvips 

OUTPUTDOCS = smslides
SUFF = ps aux dvi log toc idx bbl blg
FIGS = basetools.eps context.eps error.eps errorstate.eps  \
       tbwin.ps toolkit.eps topology.eps
MAKEEPS = toolbox.eps

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

# -P: otate the figure to  portrait  mode
%.eps: %.fig
#	fig2dev -L ps -P -z A4 $< $@
	fig2dev -L ps  -z A4 $< $@

%.eps: %.gif
	giftoppm $< | ppmtopgm | pnmtops -rle -noturn > $@ 

all: smslides.ps

smslides.ps: smslides.dvi 
	$(DVIPS) $< -o $@

smslides.dvi: smslides.tex $(FIGS) $(MAKEEPS)
	$(LATEX) $<

# What to clean. 
clean: 
	rm -f $(TEXTMP) $(MAKEEPS)

