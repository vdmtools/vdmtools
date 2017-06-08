# Install manual Makefile

PLATEX = platex
BIBTEX = bibtex
JBIBTEX = jbibtex
MAKEINDEX = makeindex
DVIPDFMX = dvipdfmx
MENDEX = mendex

OUTPUTDOCS = installsl_a4E installsl_a4J installpp_a4E installpp_a4J
SUFF = ps aux dvi log bbl blg toc idx ilg ind tex out

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

all: alleng
alljpn: installsl_a4J.pdf installpp_a4J.pdf 
alleng: installsl_a4E.pdf installpp_a4E.pdf

%J.pdf: %J.tex install.bib
	rm -f $*.aux $*.out $*.toc $@
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:J.tex=J.dvi)

%E.pdf: %E.tex install.bib
	rm -f $*.aux $*.out $*.toc $@
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:E.tex=E.dvi)

installsl_a4J.tex: install.tex 
	rm -f $@
	ifdeffilt -DVDMSL -DJPN -DA4Format $< > $@
	chmod a-w $@

installsl_a4E.tex: install.tex 
	rm -f $@
	ifdeffilt -DVDMSL -DENG -DA4Format $< > $@
	chmod a-w $@

installpp_a4J.tex: install.tex 
	rm -f $@
	ifdeffilt -DJPN -DVDMPP -DA4Format $< > $@
	chmod a-w $@

installpp_a4E.tex: install.tex 
	rm -f $@
	ifdeffilt -DENG -DVDMPP -DA4Format $< > $@
	chmod a-w $@

install.bib: 
	ln -s ../bib/ifad.bib $@

# What to clean. 
clean: 
	rm -f $(TEXTMP) install.bib

