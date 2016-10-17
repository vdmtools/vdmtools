VDMDE = vdmde
ASTDIR= ../ast
STDLIBDIR= ../stdlib-spec
EVALDIR= ../eval-spec
LN = ln -s 

LATEXFILE = statsem
SL_LATEXFILE = $(LATEXFILE:%=%-vdmsl)
PP_LATEXFILE = $(LATEXFILE:%=%-vdmpp)
RT_LATEXFILE = $(LATEXFILE:%=%-vdmrt)
#LATEX = pdflatex
LATEX = altjpdflatex
RM = rm -f
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

# Modules common for SL and PP type checker but mixed specifications
#MIXED = common_as.vdm tcaux.vdm def.vdm env.vdm err.vdm expr.vdm \
#	pat.vdm rec.vdm rep.vdm stmt.vdm test.vdm type.vdm vcm.vdm \
#        contextinfo.vdm mangle.vdm
MIXED = common_as.vdm contextinfo.vdm rep.vdm \
	expanded.vdm trtest.vdm \
	eval_sem.vdm stack-eval.vdm eval_global.vdm instructionsTP.vdm \
	eval_settings.vdm eval_def.vdm eval_free.vdm \
	eval_pat.vdm rterr.vdm eval_aux.vdm cpat.vdm debug.vdm debugTP.vdm \
        cexpr.vdm cstmt.vdm compile.vdm instructions.vdm eval_state.vdm eval_expr.vdm util.vdm

# Modules equal for SL and PP type checker
EQUAL = io.vdm

# Modules unique for the SL type checker
SLSPEC = 

# Modules unique for the PP type checker
PPSPEC =

PPSPEC_PREPRO = schdtp.vdm eval_class.vdm schd.vdm mangle.vdm

RTSPEC = timemap.vdm time.vdm timeparser.vdm timetrace.vdm valcond.vdm

# Modules require-ring preprocessing
SLMIXED = $(MIXED:%.vdm=%.vdmsl)
PPMIXED = $(MIXED:%.vdm=%.vdmpp)  $(PPSPEC_PREPRO:%.vdm=%.vdmpp)
RTMIXED = $(MIXED:%.vdm=%.vdmrt)  $(PPSPEC_PREPRO:%.vdm=%.vdmrt)

SLALL = $(SLSPEC) $(EQUAL) $(SLMIXED)
PPALL = $(PPSPEC) $(EQUAL) $(PPMIXED)
RTALL = $(RTSPEC) $(PPSPEC) $(EQUAL) $(RTMIXED)

FIG =

FIGPS = $(FIG:.fig=.ps)

################################################################################
# 
# Main targets for SL and PP type checker
#

.PHONY: default
default: 
	echo Use make SL PP dvi all
	exit 1;

# Preprocessing vdm files:
SL: .vdmde-traces-sl
PP: .vdmde-traces-pp
RT: .vdmde-traces-rt 
#$(RTALL)

# Make documentation (.tex files) in dvi format:
all:  PP SL RT dvi

dvi: $(SL_LATEXFILE).dvi  $(PP_LATEXFILE).dvi

$(STDLIBDIR)/io.vdm: 
	$(MAKE) -C $(@D) $(@F)

# Make documentation  in ps format:
SLPS: $(SL_LATEXFILE).ps 
PPPS: $(PP_LATEXFILE).ps 
RTPS: $(RT_LATEXFILE).ps 

# Make documentation  in pdf format:
SLPDF: $(SL_LATEXFILE).pdf
PPPDF: $(PP_LATEXFILE).pdf
RTPDF: $(RT_LATEXFILE).pdf 

dot: .vdmde-traces-sl .vdmde-traces-pp .vdmde-traces-rt
USER=`whoami`
#MPWD=$(shell /bin/pwd|sed -e "s .*$(USER) $(HOME) g")
MPWD=$(shell /bin/pwd)
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-traces-sl: $(SLALL)
	$(READ_CMD) $^ > $@

.vdmde-traces-pp: $(PPALL)
	$(READ_CMD) $^ > $@

.vdmde-traces-rt: $(RTALL)
	$(READ_CMD) $^ > $@

vpath %.vdm $(ASTDIR) $(STDLIBDIR)
vpath %.vdm $(EVALDIR)
vpath %.txt ../../code/errmsg
# for rep.fig
vpath %.fig  $(ASTDIR) 
#
# Link
#

link: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;



#
# Documentatin
#

%.ps : %.fig
	fig2dev -Lps $< $@ 

LATEX_SLSPECS = $(SLSPECS:%=%.tex) $(SLMIXED:%=%.tex) 
LATEX_PPSPECS = $(PPSPECS:%=%.tex) $(PPMIXED:%=%.tex) 
LATEX_RTSPECS = $(RTSPECS:%=%.tex) $(RTMIXED:%=%.tex) 
LATEX_EQUAL_SPECS = $(EQUAL:%=%.tex)
# Latex generation command
VDMDE = vdmde -lrN

# Test coverage information file 
RTINFO = rtinfo.ast

$(LATEX_SLSPECS) $(LATEX_EQUAL_SPECS) $(LATEX_PPSPECS): %.tex: % $(RTINFO)
	 $(VDMDE) $<

$(RTINFO):
	@echo "Make link to $(RTINFO) for test coverage information"

# generating SL document as dvi - file
$(SL_LATEXFILE).dvi: $(SL_LATEXFILE).tex $(LATEX_SLSPECS) $(LATEX_EQUAL_SPECS) $(FIGPS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

# generating PP document as dvi - file
$(PP_LATEXFILE).dvi: $(PP_LATEXFILE).tex $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) $(FIGPS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

# generating RT document as dvi - file
$(RT_LATEXFILE).dvi: $(RT_LATEXFILE).tex $(LATEX_RTSPECS) $(LATEX_EQUAL_SPECS) $(FIGPS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

# generating document as postscript file
$(SL_LATEXFILE).ps : $(SL_LATEXFILE).dvi
	dvips $(SL_LATEXFILE) -o

$(PP_LATEXFILE).ps : $(PP_LATEXFILE).dvi
	dvips $(PP_LATEXFILE) -o
LATEX_SLSPECS = $(SLSPECS:%=%.tex) $(SLMIXED:%=%.tex) 
LATEX_PPSPECS = $(PPSPECS:%=%.tex) $(PPMIXED:%=%.tex) 
LATEX_EQUAL_SPECS = $(EQUAL:%=%.tex)

#
# VDM-SL sources
#

$(SLMIXED): %.vdmsl: %.vdm
	-rm -f $@
#	$(IFDEFFILT) -DVDMSL -DTPINFO $< >$@	
	$(IFDEFFILT) -DVDMSL $< >$@	
	chmod a-w $@

$(SL_LATEXFILE).tex: %-vdmsl.tex: %.tex
	-rm -f $@
#	$(IFDEFFILT) -DVDMSL -DTPINFO $< >$@	
	$(IFDEFFILT) -DVDMSL $< >$@	
	chmod a-w $@

#
# VDM++ sources
#

$(PPMIXED): %.vdmpp: %.vdm
	-rm -f $@
#	$(IFDEFFILT) -DVDMPP -DTPINFO $< >$@	
	$(IFDEFFILT) -DVDMPP $< >$@	
	chmod a-w $@

$(PP_LATEXFILE).tex: %-vdmpp.tex: %.tex
	-rm -f $@
#	$(IFDEFFILT) -DVDMPP -DTPINFO $< >$@	
	$(IFDEFFILT) -DVDMPP $< >$@	
	chmod a-w $@

%.vdmrt: %.vdm
	-rm -f $@
#	$(IFDEFFILT) -DVDMPP -DVICE -DTPINFO -DRTINFO $< >$@
	$(IFDEFFILT) -DVDMPP -DVICE -DRTINFO $< >$@
	chmod a-w $@

#
# Generation of help texts
#
errmsg.vdm: errmsg.txt
	$(<D)/translateErr.pl vdm $<

#
# Updating the error comments in the specification files.
#
.PHONY: update
update:
	@if [ ! -e replaceComments.pl ]; then\
	  echo "You have to link the program replaceComments.pl from the directory .../code/errmsg";\
	  exit -1;\
	fi;
	@echo "Please remember to do a diff of the result."
	@echo "The original files are renamed to .old"
	./replaceComments.pl *.vdm

#
# Cleaning up!
#

RMEXT = aux log idx ind blg ps dvi bbl ilg toc tex

clean:
	$(RM) $(LATEX_SLSPECS) $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) 
	$(RM) *.vdmsl *.vdmpp *.vdmrt $(FIGPS) 
	$(RM) $(RMEXT:%=$(SL_LATEXFILE).%) $(RMEXT:%=$(PP_LATEXFILE).%)
	$(RM) errmsg.vdm
