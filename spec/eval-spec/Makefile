DYNDIR = eval-spec
ASTDIR = ../ast
STDLIBDIR = ../stdlib-spec
LN = ln -s 
LATEX = altjpdflatex
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

# This is use for testdir -s
.PHONY:	SL
# modules of dynamic semantic

# Modules that need preprocessing because they are mixed SL and PP
MIXED = common_as.vdm rep.vdm eval_sem.vdm eval_global.vdm \
        eval_state.vdm eval_expr.vdm debug.vdm debugTP.vdm \
        eval_def.vdm eval_aux.vdm eval_pat.vdm eval_free.vdm \
        eval_old.vdm \
        stack-eval.vdm compile.vdm instructions.vdm instructionsTP.vdm cexpr.vdm \
        cpat.vdm cstmt.vdm rterr.vdm eval_settings.vdm test.vdm \
	contextinfo.vdm util.vdm

# Modules that are used in both PP and SL, but needs no preprocessing.
EQUAL = io.vdm

# Modules that are specific for and only used in SL
SLSPEC = eval_mod.vdm

# Modules that are specific for and only used in PP
#PPSPEC = util.vdm
PPSPEC =

# Modules that are specific for and only used in PP, but must be
# preprocessed (#ifdef VICE)
PPSPEC_PREPRO = schdtp.vdm eval_class.vdm schd.vdm schdtp.vdm mangle.vdm

RTSPEC = timemap.vdm time.vdm timeparser.vdm timetrace.vdm valcond.vdm

SLMIXED = $(MIXED:%.vdm=%.vdmsl)
PPMIXED = $(MIXED:%.vdm=%.vdmpp) $(PPSPEC_PREPRO:%.vdm=%.vdmpp)
RTMIXED = $(MIXED:%.vdm=%.vdmrt) $(PPSPEC_PREPRO:%.vdm=%.vdmrt)

SLALL = $(SLSPEC) $(EQUAL) $(SLMIXED)
PPALL = $(PPSPEC) $(EQUAL) $(PPMIXED)
RTALL = $(RTSPEC) $(PPSPEC) $(EQUAL) $(RTMIXED)

dummy: 
	echo Specify a make target like SL PP RT all dot dvi

dot: .vdmde-dyn-sl .vdmde-dyn-pp .vdmde-dyn-rt

SL: .vdmde-dyn-sl
PP: .vdmde-dyn-pp
RT: .vdmde-dyn-rt

all: dot dvi

dvi: dynsemsl.dvi dynsempp.dvi dynsemrt.dvi

# Use vpath to find source files dynamically instead of having to 
# create links.

$(STDLIBDIR)/io.vdm: 
	$(MAKE) -C $(@D) $(@F)

vpath %.vdm $(ASTDIR) $(STDLIBDIR)
vpath %.fig $(ASTDIR)


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
#MPWD=$(shell /bin/pwd|sed -e "s .*$(USER) $(HOME) g")
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-dyn-sl: $(SLALL)
	$(READ_CMD) $^ > $@

.vdmde-dyn-pp: $(PPALL)
	$(READ_CMD) $^ > $@

.vdmde-dyn-rt: $(RTALL)
	$(READ_CMD) $^ > $@

RTINFO=#rtinfo.ast

$(RTINFO):
	@echo "Make link to $(RTINFO) for test coverage information"


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
#	$(IFDEFFILT) -DVDMPP -DVICE -DTPINFO $< >$@
	$(IFDEFFILT) -DVDMPP -DVICE $< >$@
	chmod a-w $@


%.vdm.tex: %.vdm $(RTINFO)
	$(VDMDE) $<

%.vdmsl.tex: %.vdmsl $(RTINFO)
	$(VDMDE) $<

%.vdmpp.tex: %.vdmpp $(RTINFO)
	$(VDMDE) $<

%.vdmrt.tex: %.vdmrt $(RTINFO)
	$(VDMDE) $<

# generating document as dvi - file
dynsemsl.dvi: $(SLTEX_ALL) dynsemsl.tex
	$(LATEX) dynsemsl
	bibtex dynsemsl
	makeindex dynsemsl.idx
	$(LATEX) dynsemsl
	$(LATEX) dynsemsl

# generating document as postscript file
dynsemsl.ps : dynsemsl.dvi
	dvips dynsemsl -o


# generating document as dvi - file
dynsempp.dvi: $(PPTEX_ALL) dynsempp.tex
	$(LATEX) dynsempp
	bibtex dynsempp
	makeindex dynsempp.idx
	$(LATEX) dynsempp
	$(LATEX) dynsempp

# generating document as dvi - file
dynsemrt.pdf: $(RTTEX_ALL) dynsemrt.tex
	$(LATEX) dynsemrt
	bibtex dynsemrt
	makeindex dynsemrt.idx
	$(LATEX) dynsemrt
	$(LATEX) dynsemrt

# generating document as postscript file
dynsempp.ps : dynsempp.dvi
	dvips dynsempp -o

clean:
	rm -f *.vdm.tex
	rm -f *.vdmsl *.vdmsl.tex
	rm -f *.vdmpp *.vdmpp.tex
	rm -f *.vdmrt *.vdmrt.tex
	rm -f dynsemsl.aux dynsemsl.log dynsemsl.idx
	rm -f dynsemsl.ind dynsemsl.ilg dynsemsl.blg
	rm -f dynsempp.aux dynsempp.log dynsempp.idx
	rm -f dynsempp.ind dynsempp.ilg dynsempp.blg

