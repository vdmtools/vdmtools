# Install manual Makefile

PLATEX = platex
BIBTEX = bibtex
JBIBTEX = jbibtex
MAKEINDEX = makeindex
DVIPDFMX = dvipdfmx
MENDEX = mendex
EBB = ebb

OUTPUTDOCS = installsl_a4E installsl_a4J installpp_a4E installpp_a4J installvice_a4E installvice_a4J
SUFF = ps aux dvi log bbl blg toc idx ilg ind tex out

FIGDIR = figures

%.png:
	cd $(FIGDIR); $(EBB) $@

DUMPS = install_pp_start.png install_sl_start.png install_vice_start.png \
	install_pp_second.png install_sl_second.png install_vice_second.png \
	install_pp_third.png install_sl_third.png install_vice_third.png \
	install_pp_bar.png install_sl_bar.png install_vice_bar.png \
	install_pp_finish.png install_sl_finish.png install_vice_finish.png

PNGFILES = $(DUMP)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

all: alljpn
alljpn: installsl_a4J.pdf installpp_a4J.pdf installvice_a4J.pdf

%J.pdf: %J.tex install.bib $(PNGFILES) $(EPSFORMING)
	rm -f $*.aux $*.out $*.toc $@
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:J.tex=J.dvi)

installsl_a4J.tex: install.tex $(PNGFILES)
	rm -f $@
	ifdeffilt -DVDMSL -DJPN -DA4Format $< > $@
	chmod a-w $@

installpp_a4J.tex: install.tex 
	rm -f $@
	ifdeffilt -DJPN -DVDMPP -DA4Format $< > $@
	chmod a-w $@

installvice_a4J.tex: install.tex 
	rm -f $@
	ifdeffilt -DJPN -DVDMVICE -DA4Format $< > $@
	chmod a-w $@
	
install.bib: 
	ln -s ../bib/ifad.bib $@

# What to clean. 
clean: 
	rm -f $(TEXTMP) $(EPSFORMING) install.bib

