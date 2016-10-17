POG_AUX = ../pog-aux-spec
POG_PRETTY = ../pog-pretty-spec
ASTDIR = ../ast
SSDIR = ../ss-spec
STDLIBDIR = ../stdlib-spec
LN = ln -s 
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

LATEXFILESL = pogsl
LATEXFILEPP = pogpp
LATEXFILERT = pogrt
LATEX = latex
PDFLATEX = pdflatex
MAKEINDEX = makeindex
BIBTEX = bibtex

# This is use for testdir -s
.PHONY:	SL
# modules of dynamic semantic

# Modules that need preprocessing because they are mixed SL and PP
MIXED = common_as.vdm rep.vdm contextinfo.vdm \
        tcaux.vdm def.vdm env.vdm err.vdm expr.vdm pat.vdm stmt.vdm \
        type.vdm vcm.vdm test-pog.vdm \
        pogTP.vdm type2rep.vdm\
        pog-def.vdm pog-stmt.vdm pog-expr.vdm \
	pog-patterns.vdm \
	pattern-identifiers.vdm \
	casting-patterns2expr.vdm \
	patterns2pure-patterns.vdm \
        pp-mediator.vdm pp-traverse-def.vdm \
	pp-traverse-expr.vdm \
        pp-target-def.vdm trf-rep2type.vdm \
	context-environment.vdm \
        check-subtype.vdm \
	pog-inv-generation.vdm \
	pog-generation.vdm \
	pp-traverse-patterns.vdm \
	pp-target-patterns.vdm \
        spec-ast.vdm context-info-auxiliary.vdm \
	rec.vdm util.vdm

# Modules that are used in both PP and SL, but needs no preprocessing.
EQUAL = fresh-variables.vdm \
	pog-mediator.vdm \
	pp-target-expr.vdm \
	pp-target-type.vdm \
	pp-target-po.vdm \
	pp-traverse-type.vdm \
	pp-traverse-po.vdm \
        errmsg.vdm

# Modules that are specific for and only used in SL
SLSPEC =

SSSPEC = 

PPSPEC_PREPRO = mangle.vdm

SLMIXED   = $(MIXED:%.vdm=%.vdmsl)
PPMIXED   = $(MIXED:%.vdm=%.vdmpp) $(PPSPEC_PREPRO:%.vdm=%.vdmpp)
RTMIXED   = $(MIXED:%.vdm=%.vdmrt) $(PPSPEC_PREPRO:%.vdm=%.vdmrt)

SLALL   = $(SLSPEC) $(EQUAL) $(SLMIXED) $(SSSPEC)
PPALL   = $(SLSPEC) $(EQUAL) $(PPMIXED) $(SSSPEC)
RTALL   = $(SLSPEC) $(EQUAL) $(RTMIXED) $(SSSPEC)

#
# Generation of help texts
#
errmsg.vdm: errmsg.txt
	$(<D)/translateErr.pl vdm $<

dummy: 
	echo Specify a make target like SL PP RT dvipp dvisl dvirt 

dot: SL PP RT

SL: .vdmde-pog-sl
PP: .vdmde-pog-pp
RT: .vdmde-pog-rt

dvi: $(LATEXFILESL).dvi $(LATEXFILEPP).dvi $(LATEXFILERT).dvi

pdf: $(PDFFILESL).pdf $(PDFFILEPP).pdf $(PDFFILERT).pdf

# Make documentation  in ps format:
ps: $(LATEXFILESL).ps $(LATEXFILEPP).ps $(LATEXFILERT).ps

all: dot ps pdf 

# Use vpath to find source files dynamically instead of having to 
# create links.

#vpath %.vdmsl $(POG_AUX) $(POG_PRETTY)
#vpath %.vdmpp $(POG_AUX) $(POG_PRETTY)
vpath %.vdm $(ASTDIR) $(STDLIBDIR) $(POG_AUX) $(POG_PRETTY)
vpath %.fig $(ASTDIR)
vpath %.vdm $(SSDIR)
vpath %.txt ../../code/errmsg

#LATEX_SPECS = $(SLSPEC:%=%.tex)

# Test coverage information file 
RTINFO = # rtinfo.ast

$(RTTEX_ALL) $(PPTEX_ALL) $(SLTEX_ALL): %.tex: % $(RTINFO)
	 $(VDMDE) $<
	 cat $<.tex | sed -e's/\\color{not_covered}/\\color{not_covered}\\index{Uncovered Construct}/g' > $<.tmp
	 mv -f $<.tmp $<.tex

# generating POG document as dvi - file
$(LATEXFILESL).dvi: $(LATEXFILESL).tex $(SLTEX_ALL)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

$(LATEXFILEPP).dvi: $(LATEXFILEPP).tex $(PPTEX_ALL)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

$(LATEXFILERT).dvi: $(LATEXFILERT).tex $(RTTEX_ALL)
	$(LATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

$(LATEXFILESL).ps : $(LATEXFILESL).dvi
	dvips $(LATEXFILESL) -o

$(LATEXFILEPP).ps : $(LATEXFILEPP).dvi
	dvips $(LATEXFILEPP) -o

$(LATEXFILERT).ps : $(LATEXFILERT).dvi
	dvips $(LATEXFILERT) -o

$(LATEXFILESL).tex: $(LATEXFILESL).tex
	-rm -f $@
#	$(IFDEFFILT) -DVDMSL -DA4Format -DTPINFO $< >$@	
	$(IFDEFFILT) -DVDMSL -DA4Format $< >$@	
	chmod a-w $@

$(LATEXFILEPP).tex: $(LATEXFILEPP).tex
	-rm -f $@
#	$(IFDEFFILT) -DVDMPP -DA4Format -DTPINFO $< >$@	
	$(IFDEFFILT) -DVDMPP -DA4Format $< >$@	
	chmod a-w $@

$(LATEXFILERT).tex: $(LATEXFILERT).tex
	-rm -f $@
#	$(IFDEFFILT) -DVICE -DVDMPP -DA4Format -DTPINFO $< >$@	
	$(IFDEFFILT) -DVICE -DVDMPP -DA4Format $< >$@	
	chmod a-w $@

$(LATEXFILESL).pdf: $(LATEXFILESL).tex $(SLTEX_ALL)
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

$(LATEXFILEPP).pdf: $(LATEXFILEPP).tex $(PPTEX_ALL)
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

$(LATEXFILERT).pdf: $(LATEXFILERT).tex $(RTTEX_ALL)
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

link linkall: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;

SLTEX_ALL = $(SLALL:%=%.tex)
PPTEX_ALL = $(PPALL:%=%.tex)
RTTEX_ALL = $(RTALL:%=%.tex)

# latex generation command. requires a new vdmde that puts
# output files in current working directory!
VDMDE = vdmde -lrN

# ETN: use /bin/pwd below to get real path!
USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-pog-sl: $(SLALL)
	$(READ_CMD) $^ > $@

.vdmde-pog-pp: $(PPALL)
	$(READ_CMD) $^ > $@

.vdmde-pog-rt: $(RTALL)
	$(READ_CMD) $^ > $@

%.vdmsl: %.vdm
	-rm -f $@
#	$(IFDEFFILT) -DVDMSL -DTPINFO $< >$@
	$(IFDEFFILT) -DVDMSL $< >$@
	chmod a-w $@

%.vdmpp: %.vdm
	-rm -f $@
#	$(IFDEFFILT) -DVDMPP -DTPINFO $< >$@
	$(IFDEFFILT) -DVDMPP $< >$@
	chmod a-w $@

%.vdmrt: %.vdm
	-rm -f $@
#	$(IFDEFFILT) -DVICE -DVDMPP -DTPINFO $< >$@
	$(IFDEFFILT) -DVICE -DVDMPP $< >$@
	chmod a-w $@

clean:
	rm -f *.vdmsl *.vdmsl.tex *.vdmsl.aux
	rm -f *.vdmpp *.vdmpp.tex *.vdmpp.aux
	rm -f *.vdmrt *.vdmrt.tex *.vdmrt.aux
	rm -f *.vdm.tex *.vdm.aux
	rm -f $(LATEXFILESL).aux $(LATEXFILESL).log $(LATEXFILESL).idx
	rm -f $(LATEXFILESL).ind $(LATEXFILESL).ilg $(LATEXFILESL).blg
	rm -f $(LATEXFILEPP).aux $(LATEXFILEPP).log $(LATEXFILEPP).idx
	rm -f $(LATEXFILEPP).ind $(LATEXFILEPP).ilg $(LATEXFILEPP).blg
	rm -f $(LATEXFILERT).aux $(LATEXFILERT).log $(LATEXFILERT).idx
	rm -f $(LATEXFILERT).ind $(LATEXFILERT).ilg $(LATEXFILERT).blg







