#
# $Id: Makefile,v 1.2 2006/04/19 05:06:48 vdmtools Exp $
#

VSRC = Event.vpp \
       InterruptEvent.vpp \
       NetworkEvent.vpp \
       AbstractTask.vpp \
       BasicTask.vpp \
       EnvironmentTask.vpp \
       MMIHandleKeyPressOne.vpp \
       MMIHandleKeyPressTwo.vpp \
       MMIUpdateScreenVolume.vpp \
       MMIUpdateScreenAddress.vpp \
       MMIUpdateScreenTMC.vpp \
       RadioAdjustVolume.vpp \
       RadioHandleTMC.vpp \
       NavigationDatabaseLookup.vpp \
       NavigationDecodeTMC.vpp \
       VolumeKnob.vpp \
       InsertAddress.vpp \
       TransmitTMC.vpp \
       EventDispatcher.vpp \
       RadNavSys.vpp \
       Logger.vpp \
       AbstractTaskEvent.vpp

TSRC = $(addsuffix .tex, $(foreach f, $(VSRC), $f))

#LATEX = pdflatex
LATEX = dotexshop

%.vpp.tex: %.vpp
	perl replace.pl $<
	
all: improve

improve: $(TSRC) improve.tex
	$(LATEX) improve
	bibtex improve
	$(LATEX) improve
	$(LATEX) improve

clean:
	rm -rf *.aux *.log *.bbl *.blg *.dvi *~ *.toc
	rm -rf *.bak vdmprofile *.vpp.tex *.ps

realclean: clean
	rm -rf improve.pdf logfile mytrace.txt
