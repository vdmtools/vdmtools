#
# Makefile for the dependency module.
# 
# 
# make link : creates links to all needed files and creates the script
#             files .vdm-dep-sl and .vdm-dep-pp
#
# make PP :   preprocesses mixed specifications and generates a specification
#             specific for VDM++
#
# make PPTEX: generates documentation in .dvi format
#
# make PPPS:  generates documentation in postscript

ASTDIR= ../ast
LN = ln -sv 
LATEXFILE = dependency
SL_LATEXFILE = $(LATEXFILE:%=%-vdmsl)
PP_LATEXFILE = $(LATEXFILE:%=%-vdmpp)
LATEX = latex
RM = rm -f
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

# Modules written as mixed specifications
MIXED = common_as.vdm rep.vdm contextinfo.vdm

# Modules equal for SL and PP
EQUAL = 

# Specific modules for dependencies in VDM-SL
SLSPECS = 

# Specific modules for dependencies in VDM++
PPSPECS = dep.vdm

# Modules require-ring preprocessing
SLMIXED = $(MIXED:%.vdm=%.vdmsl)
PPMIXED = $(MIXED:%.vdm=%.vdmpp)

SLALL = $(SLSPECS) $(EQUAL) $(SLMIXED)
PPALL = $(PPSPECS) $(EQUAL) $(PPMIXED)


###############################################################################
#
# Main targets for latex generation and preprocessing
#

.PHONY: default
default: 
	echo Use make SL PP dvi all
	exit 1;

all: SL PP dvi

# Preprocessing vdm files:
SL: .vdmde-dep-sl
PP: .vdmde-dep-pp

dot: .vdmde-dep-sl .vdmde-dep-pp

# Make documentation (.tex files) in dvi format:
dvi: $(SL_LATEXFILE).dvi $(PP_LATEXFILE).dvi

# Make documentation  in ps format:
SLPS: $(SL_LATEXFILE).ps 
PPPS: $(PP_LATEXFILE).ps 

#
################################################################################

linkall: common_as.vdm rep.vdm dep.vdm dependency.tex

vpath %.vdm $(ASTDIR)
vpath %.tex $(ASTDIR)

LATEX_SLSPECS = $(SLSPECS:%=%.tex) $(SLMIXED:%=%.tex) 
LATEX_PPSPECS = $(PPSPECS:%=%.tex) $(PPMIXED:%=%.tex) 
LATEX_EQUAL_SPECS = $(EQUAL:%=%.tex)
# Latex generation command
VDMDE = vdmde -lrN

# Test coverage information file 
RTINFO = 

$(LATEX_SLSPECS) $(LATEX_EQUAL_SPECS) $(LATEX_PPSPECS): %.tex: % $(RTINFO)
	 $(VDMDE) $<

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

# Make .vdmde-dep-sl and .vdmde-dep-pp for testing the specification

USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD:=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD:=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-dep-sl: $(SLALL)
	$(READ_CMD) $^ > $@

.vdmde-dep-pp: $(PPALL)
	$(READ_CMD) $^ > $@


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

RMEXT = aux log idx ind blg ps dvi bbl ilg toc tex

clean:
	$(RM) $(LATEX_SLSPECS) $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) 
	$(RM) *.vdmsl *.vdmpp .vdmde-dep-sl .vdmde-dep-pp
	$(RM) $(RMEXT:%=$(SL_LATEXFILE).%) $(RMEXT:%=$(PP_LATEXFILE).%)


