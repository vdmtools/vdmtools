# $Id: Makefile,v 1.10 2006/01/25 09:38:12 vdmtools Exp $

LATEX = latex
PLATEX = platex
DVIPDFMX = dvipdfmx
DVIPDFM = dvipdfm
BIBTEX = bibtex
JBIBTEX = jbibtex
MAKEINDEX = makeindex
MENDEX = mendex
EBB = ebb

OUTPUTDOCS = usermansl_a4E usermanpp_a4E usermanvice_a4E \
	usermansl_a4J usermanpp_a4J usermanvice_a4J \
	glossary_en glossary_jp
SUFF = ps aux dvi log bbl blg toc idx ilg ind tex out

FIGDIR=figures

%.png:
	cd $(FIGDIR); $(EBB) $@

DUMPS =	guitcov-sl.png guitcov-pp.png \
	toolOptions-ep.png toolOptions-font.png  \
	moduleView.png toolOptions-epENG.png \
	vdmView.png interpreterWindow.png jcgOptions.png \
	startgui-sl.png addFiles-sl.png addedFiles-sl.png \
	startgui-pp.png addFiles-pp.png addedFiles-pp.png \
	addFiles-slENG.png addFiles-ppENG.png \
	errorList-sl.png typeError1-sl.png typeError2-sl.png \
	errorList-pp.png typeError1-pp.png typeError2-pp.png \
	sourceWindow-sl.png sourceWindow-type-sl.png \
	sourceWindow-pp.png sourceWindow-type-pp.png \
	debugging-sl.png dynamicTCError-sl.png \
	debugging-pp.png dynamicTCError-pp.png \
	projectView-sl.png evalExpr-sl.png testCov-sl.png\
	projectView-pp.png evalExpr-pp.png testCov-pp.png\
	projectView-slENG.png projectView-ppENG.png \
	projectMenuToolbar-sl.png actionsMenuToolbar-sl.png \
	projectMenuToolbar-pp.png actionsMenuToolbar-pp.png \
	ppOptions-sl.png tcOptions-sl.png \
	ppOptions-pp.png tcOptions-pp.png \
	ppOptions-vice.png tcOptions-vice.png \
	interpreterOptions-sl.png ccgOptions-sl.png\
	interpreterOptions-pp.png ccgOptions-pp.png \
	interpreterOptions-vice.png ccgOptions-vice.png \
	integWin-pp.png integWin-sl.png viceOptions-vice.png \
	testenv.png vdmtools_sl.png vdmtools_pp.png \
	exeoverview.png detailedexe.png jcgOptions-vice.png \
	cpuarchitecture.png
        
ICONS =	help.png qt.png plus.png minus.png \
	projectnew.png projectsave.png projectsaveas.png \
	syntaxcheck.png syntaxcheckdone.png \
	syntaxcheckerror.png syntaxcheckmodified.png \
	typecheck.png typecheckdone.png typecheckerror.png \
	externaleditor.png fileclose.png filecloseall.png \
	interpreter.png runI.png continueI.png upI.png downI.png \
	singlestepI.png projectoptions.png pauseI.png \
	prettyprint.png prettyprintdone.png prettyprinterror.png \
	stepI.png stopI.png load.png log.png error.png \
	cplusplus.png cplusplusdone.png cpluspluserror.png \
	java.png javadone.png javaerror.png stepintoI.png\
	java2vdm.png java2vdmdone.png java2vdmerror.png \
	browser.png tooloptions.png source.png \
	up.png down.png left.png right.png checkmark.png \
	filter.png integritycheck.png

PNGFILES = $(DUMPS) $(ICONS)

TEXTMP = $(foreach suf,$(SUFF), $(foreach b, $(OUTPUTDOCS), $(b).$(suf))) 

all: alleng
alljpn: usermansl_a4J.pdf usermanpp_a4J.pdf usermanvice_a4J.pdf
alleng: usermansl_a4E.pdf usermanpp_a4E.pdf usermanvice_a4E.pdf

#%J.pdf: %J.tex user_man.bib glossary_jp.tex $(PNGFILES) $(EPSFROMFIG)
%J.pdf: %J.tex glossary_jp.tex $(PNGFILES) $(EPSFROMFIG)
	rm -f $*.aux $*.out $*.toc $@
	$(PLATEX) $<
	$(JBIBTEX) $(<:J.tex=J)
	$(MENDEX) $*J.idx
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:J.tex=J.dvi)

#%E.pdf: %E.tex user_man.bib glossary_en.tex $(PNGFILES)
%E.pdf: %E.tex glossary_en.tex $(PNGFILES)
	rm -f $*.aux $*.out $*.toc $@
	$(PLATEX) $<
	$(BIBTEX) $(<:E.tex=E)
	$(MAKEINDEX) $*E.idx
	$(PLATEX) $<
	$(PLATEX) $<
	$(DVIPDFMX) $(<:E.tex=E.dvi)

usermansl_a4E.tex: user_man.tex
	rm -f $@
	ifdeffilt -DVDMSL -DENG -DA4Format $< > $@
	chmod a-w $@

usermansl_a4J.tex: user_man.tex
	rm -f $@
	ifdeffilt -DVDMSL -DJPN -DA4Format $< > $@
	chmod a-w $@

usermanpp_a4E.tex: user_man.tex
	rm -f $@
	ifdeffilt -DVDMPP -DPPONLY -DENG -DA4Format $< > $@
	chmod a-w $@

usermanpp_a4J.tex: user_man.tex
	rm -f $@
	ifdeffilt -DVDMPP -DPPONLY -DJPN -DA4Format $< > $@
	chmod a-w $@

usermanvice_a4E.tex: user_man.tex
	rm -f $@
	ifdeffilt -DVICEMAN -DENG -DVDMPP -DA4Format $< > $@
	chmod a-w $@

usermanvice_a4J.tex: user_man.tex
	rm -f $@
	ifdeffilt -DVICEMAN -DJPN -DVDMPP -DA4Format $< > $@
	chmod a-w $@

glossary_en.tex: glossary.tex
	rm -f $@
	ifdeffilt -DENG $< > $@
	chmod a-w $@

glossary_jp.tex: glossary.tex
	rm -f $@
	ifdeffilt -DJPN $< > $@
	chmod a-w $@

#user_man.bib: 
#	cp ../bib/ifad.bib $@

# What to clean. 
clean: 
#	rm -f $(TEXTMP) $(EPSFROMFIG) user_man.bib *.pbm $(FIGDIR)/*.bb
	rm -f $(TEXTMP) $(EPSFROMFIG) *.pbm $(FIGDIR)/*.bb
