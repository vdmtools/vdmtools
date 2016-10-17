ASTDIR= ../ast
SSDIR = ../ss-spec
LN = ln -s 
LATEXFILE = straightforward
SL_LATEXFILE = $(LATEXFILE:%=%-vdmsl)
PP_LATEXFILE = $(LATEXFILE:%=%-vdmpp)
LATEX = latex
RM = rm -f
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

# Modules common for SL and PP code generators but mixed specifications
MIXED = common_as.vdm contextinfo.vdm rep.vdm\
	mod_fctdef.vdm mod_expr.vdm mod_stmt.vdm mod_typedef.vdm \
        mod_valdef.vdm mod_bcppast.vdm mod_vdm_ds.vdm \
        mod_cgaux.vdm mod_tpgen.vdm mod_cgmain.vdm \
        mod_bcppnilast.vdm mod_cppast.vdm mod_patmat.vdm \
        tcaux.vdm def.vdm env.vdm err.vdm expr.vdm pat.vdm stmt.vdm \
        type.vdm vcm.vdm rec.vdm \
        util.vdm \
	test.vdm

PP_MIXED = $(MIXED) mangle.vdm

# Modules equal for SL and PP code generators
EQUAL = mod_cgtp.vdm errmsg.vdm
#EQUAL = errmsg.vdm

# Modules unique for the SL-Code Generator
SLSPECS = mod_mod.vdm mod_statedef.vdm 

# Modules unique for the PP-Code Generator
#PPSPECS = mod_fvd.vdm mod_class.vdm mod_ti.vdm mod_conc.vdm
PPSPECS = mod_fvd.vdm mod_class.vdm mod_conc.vdm
RTSPECS := $(PPSPECS)

# Modules require-ring preprocessing
SLMIXED =  $(MIXED:%.vdm=%.vdmsl)
PPMIXED =  $(PP_MIXED:%.vdm=%.vdmpp)
RTMIXED =  $(PP_MIXED:%.vdm=%.vdmrt)
SLALL = $(SLSPECS) $(EQUAL) $(SLMIXED)
PPALL = $(PPSPECS) $(EQUAL) $(PPMIXED)
RTALL = $(RTSPECS) $(EQUAL) $(RTMIXED)

errmsg.vdm: errmsg.txt
	$(<D)/translateErr.pl vdm $<

################################################################################
# 
# Main targets for SL and PP code generators. 
#
# Make links in current directory to specifications in sub-dirs and
# create script files .vdmde-sl and .vdmde-pp:


.PHONY: default
default: 
	echo Use make SL PP RT dvi all
	exit 1;

link linkall: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;

all: SL PP RT dvi

# Preprocessing vdm files:
SL: .vdmde-cg-sl
PP: .vdmde-cg-pp
RT: .vdmde-cg-rt

dot: .vdmde-cg-sl .vdmde-cg-pp .vdmde-cg-rt

dvi: $(PP_LATEXFILE).dvi $(SL_LATEXFILE).dvi 

# Make documentation  in ps format:
SLPS: $(SL_LATEXFILE).ps 
PPPS: $(PP_LATEXFILE).ps 

#
################################################################################

vpath %.vdm $(ASTDIR)
vpath %.tex $(ASTDIR)
vpath %.vdm $(SSDIR)
vpath %.txt ../../code/errmsg

LATEX_SLSPECS = $(SLSPECS:%=%.tex) $(SLMIXED:%=%.tex) 
LATEX_PPSPECS = $(PPSPECS:%=%.tex) $(PPMIXED:%=%.tex) 
LATEX_EQUAL_SPECS = $(EQUAL:%=%.tex)
# Latex generation command
VDMDE = vdmde -lrN

# Test coverage information file 
RTINFO = # rtinfo.ast

$(LATEX_SLSPECS) $(LATEX_EQUAL_SPECS) $(LATEX_PPSPECS): %.tex: % $(RTINFO)
	 $(VDMDE) $<
	 cat $<.tex | sed -e's/\\color{not_covered}/\\color{not_covered}\\index{Uncovered Construct}/g' > $<.tmp
	 mv -f $<.tmp $<.tex

$(RTINFO):
	@echo "Make link to $(RTINFO) for test coverage information"

# generating SL document as dvi - file
$(SL_LATEXFILE).dvi: $(SL_LATEXFILE).tex $(LATEX_SLSPECS) $(LATEX_EQUAL_SPECS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

# generating PP document as dvi - file
$(PP_LATEXFILE).dvi: $(PP_LATEXFILE).tex $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) 
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

USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-cg-sl: $(SLALL)
	$(READ_CMD) $^ > $@

.vdmde-cg-pp: $(PPALL)
	$(READ_CMD) $^ > $@

.vdmde-cg-pp-win: .vdmde-cg-pp
	cat $^ | sed -e "s /home //hermes g" > $@

.vdmde-cg-sl-win: .vdmde-cg-sl
	cat $^ | sed -e "s /home //hermes g" > $@

.vdmde-cg-rt: $(RTALL)
	$(READ_CMD) $^ > $@

#
# VDM-SL sources
#

$(SLMIXED): %.vdmsl: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMSL -DTPINFO $< >$@	
	chmod a-w $@

$(SL_LATEXFILE).tex: %-vdmsl.tex: %.tex
	-rm -f $@
	$(IFDEFFILT) -DVDMSL -DTPINFO $< >$@	
	chmod a-w $@

#
# VDM++ sources
#

$(PPMIXED): %.vdmpp: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMPP -DTPINFO $< >$@	
	chmod a-w $@

$(PP_LATEXFILE).tex: %-vdmpp.tex: %.tex
	-rm -f $@
	$(IFDEFFILT) -DVDMPP -DTPINFO $< >$@	
	chmod a-w $@

# RT preprocessing

$(RTMIXED): %.vdmrt: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMPP -DTPINFO -DVICE $< >$@	
	chmod a-w $@


RMEXT = aux log idx ind blg ps dvi bbl ilg toc tex

clean:
	$(RM) $(LATEX_SLSPECS) $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) 
	$(RM) *.vdmsl *.vdmpp *.vdmrt $(DOTSL) $(DOTPP)
	$(RM) $(RMEXT:%=$(SL_LATEXFILE).%) $(RMEXT:%=$(PP_LATEXFILE).%)
