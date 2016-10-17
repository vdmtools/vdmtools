# $Id: Makefile,v 1.16 2006/02/21 05:06:30 vdmtools Exp $

PLATEX = platex
DVIPDFMX = dvipdfmx
BIBTEX = pbibtex
EBB = ebb

OUTPUTDOCS = roselinkpp_a4 roselinkpp_letter 
SUFF = ps aux dvi log bbl blg toc idx ilg ind pdf tex out

FIGDIR = figures

%.eps: %.png
	convert $< $@	

%.png:
	cd $(FIGDIR); $(EBB) $@

PNG = button1rosemanual.png button2rosemanual.png button3rosemanual.png \
      button4rosemanual.png button5rosemanual.png rose.png \
      invokeRose.png roseWindow.png sortClasses.png updatedRtf.png \
      classDiagram.png relations.png sortMachineClass.png inheritance.png \
      qualifiedAssociations.png noncompliance.png classRepository.png \
      sortervdm.png sorteruml.png explSortUml.png explSortVdm.png \
      classListUml.png warning.png bothChanged.png differences.png \
      stereotype.png queueClass.png refs.png multiRefs.png \
      thisclass1.png thisclass2.png thisclass3.png thisclass4.png \
      thisclass5.png thisclass6.png thisclass7.png vppuml_arch.png seldir.png
      

# These files are automatically created. 
#EPSFROMPNG = $(patsubst %.png, %.eps, $(PNG))
EPSFROMPNG = $(PNG)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

all:roselinkpp_a4E.pdf

roselinkpp_a4E.pdf: roselinkpp_a4E.tex ifad.bib $(EPSFROMPNG) 
	rm -f roselinkpp_a4E.aux roselinkpp_a4E.out roselinkpp_a4E.toc roselinkpp_a4E.pdf
	$(PLATEX) $<
	$(BIBTEX) $(<:E.tex=E)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:E.tex=E.dvi)

roselinkpp_a4E.tex: roselink_man.tex
	rm -f $@
	ifdeffilt -DVDMPP -DA4Format $< > $@
	chmod a-w $@

ifad.bib:
	cp ../bib/ifad.bib .

# What to clean. 
clean: 
	rm -f $(TEXTMP) roselinkpp.ps roselinkpp.pdf roselinkpp_book.ps $(EPSFROMPNG) $(PDFFROMEPS) ifad.bib
	rm -f *.dvi *.ind *.ilg *.idx *.log *.out *.bbl *.aux *.toc *.blg *E.tex $(FIGDIR)/*.bb

realclean: clean
	rm -f *J.pdf

