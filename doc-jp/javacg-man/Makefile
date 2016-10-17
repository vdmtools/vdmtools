#################################################
# Java Code Generator manual Makefile
#################################################

PLATEX = platex
DVIPDFMX = dvipdfmx
BIBTEX = bibtex
EBB = ebb

OUTPUTDOCS = javacgmanpp_a4J javacgmanvice_a4J
SUFF = ps aux dvi log toc idx blg bbl tex out

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

EXAMPLES = example/Sort.vpp example/DoSort.java example/MainSort.java \
            manexamples/interfaces.vpp

FIGDIR = figures
DUMPS = dosort.png options.png optionsmenu.png toolbox.png \
           warning.png interfaces1.png interfaces2.png 
ICONS = java.png
PNGFILES = $(DUMPS) $(ICONS)

EPS_FROM_PNG = $(PNGFILES)

%.png:
	cd $(FIGDIR); $(EBB) $@

all:  javacgmanpp_a4J.pdf javacgmanvice_a4J.pdf
#all:  javacgmanpp_a4.ps  javacgmanpp_letter.ps \
#      javacgmanpp_a4.pdf javacgmanpp_letter.pdf


#
# VDM++ Code Generator Manual
#
javacgmanpp_a4J.tex: javacgman.tex #$(EXAMPLES)
	rm -f $@
	ifdeffilt -DVDMPP -DA4Format $< > $@
	chmod a-w $@

javacgmanvice_a4J.tex: javacgman.tex #$(EXAMPLES)
	rm -f $@
	ifdeffilt -DVDMPP -DVICEMAN -DA4Format $< > $@
	chmod a-w $@

javacgmanpp_a4J.pdf: javacgmanpp_a4J.tex ifad.bib cp_example $(EPS_FROM_PNG)
	rm -f $@
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

javacgmanvice_a4J.pdf: javacgmanvice_a4J.tex ifad.bib cp_example $(EPS_FROM_PNG)
	rm -f $@
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

ifad.bib:
	cp ../bib/ifad.bib .

cp_example:
	cp -pr ../../examples/pp/javasort example

# What to clean. 
clean: 
	rm -f $(TEXTMP) javacgmanpp_book.ps $(EPSFILES) ifad.bib
	rm -f $(FIGDIR)/*.bb
	rm -rf example
