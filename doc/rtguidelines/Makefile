
PLATEX = platex
DVIPDFMX = dvipdfmx
BIBTEX = bibtex
JBIBTEX = jbibtex
MAKEINDEX = makeindex
MENDEX=mendex
EBB = ebb

OUTPUTDOCS = vicedevelopmentprocessE
SUFF = ps aux dvi log bbl blg toc idx ilg ind tex out
#SUFF = ps aux dvi log bbl blg toc idx ilg ind pdf tex out

FIGDIR=figures

EPSFROMFIG = $(FIGDIR)/vdmtools_pp.eps $(FIGDIR)/vdmtools_sl.eps $(FIGDIR)/testenv.eps

%.eps: %.fig
	fig2dev -L eps $< $@

periodic.pdf: 
	convert $(FIGDIR)/$@ $(FIGDIR)/periodic.eps

%.jpg:
	cd $(FIGDIR); $(EBB) $@

%.png:
	cd $(FIGDIR); $(EBB) $@

JPGFILES =	lifecycle.jpg reqcapture.jpg generalusecase.jpg \
		generalseqdia2.jpg generalstatedia.jpg VDM++levelsofmodels.jpg \
		VDM++levelsofmodels.jpg contextdia.jpg flareseqs.jpg \
		responses.jpg analysismissile.jpg bespokemissile.jpg 
PNGFILES =	generalclassdiag.png CMusecases.png CMstate.png \
		seqCMclassdiag.png concurCMclassdiag.png viceCMclassdiag.png \
		cpuarchitecture.png exeoverview.png detailedexe.png \
		freshdata.png freshdataseqdiag.png timestamp.png \
		timestampseqdiag.png
PDFFILES =	periodic.pdf

CONVFILES = $(JPGFILES) $(PNGFILES) $(PDFFILES)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

all: vicedevelopmentprocessE.pdf

vicedevelopmentprocessE.pdf: vicedevelopmentprocessE.tex $(CONVFILES)
	rm -f $*.aux $*.out $*.toc $@
	$(PLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:.tex=.dvi)

vicedevelopmentprocessE.tex: Vice_Development_Process.tex
	cp Vice_Development_Process.tex vicedevelopmentprocessE.tex

# What to clean. 
clean: 
	rm -f $(TEXTMP) ifad.bib \
		*.aux *.pbm $(FIGDIR)/*.bb $(FIGDIR)/*.eps
