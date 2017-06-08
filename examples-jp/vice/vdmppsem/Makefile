# latex generation command. requires a new vdmde that puts
# output files in current working directory!

VDMDE = vdmde.exe -lN

LSRC1 =	vdmppsem.tex \
	vdmppsem.bib 

LSRC2 = fme06.tex \
	abstract.tex \
	intro.tex \
	istundsoll.tex \
	casestudy.tex \
	semantics.vdm \
	discussion.tex \
	dan.bib

VSRC1 =  radnav/radnav.tex 

PICS1 =	

TSRC1 = $(addsuffix .tex, $(foreach f, $(VSRC1), $f))

%.vdm.tex: %.vdm 
	$(VDMDE) $<

%.tex.tex: %.tex
	perl replace.pl $<
	
all: vdmppsem fme06

fme06: $(LSRC2) semantics.vdm.tex
	latex fme06
	bibtex fme06
	latex fme06
	latex fme06
	dvipdf fme06
	
vdmppsem: $(TSRC1) $(LSRC1) $(PICS1)
	latex vdmppsem
	bibtex vdmppsem
	latex vdmppsem
	latex vdmppsem
	dvipdf vdmppsem

check: vdmppsem.tex
	aspell -t -c vdmppsem.tex

check06: $(LSRC2)
	aspell -t -c fme06.tex
	aspell -t -c abstract.tex
	aspell -t -c intro.tex
	aspell -t -c istundsoll.tex
	aspell -t -c casestudy.tex
	aspell -t -c semantics.vdm
	aspell -t -c discussion.tex

clean:
	rm -rf *.aux *.log *.bbl *.blg *.dvi *~ *.bak vdmprofile *.vdm.tex *.ps
	rm -rf radnav/*.tex.* radnav/*~ radnav/*.msg radnav/*.aux

realclean: clean
	rm -rf vdmppsem.pdf
	rm -rf fme06.pdf
