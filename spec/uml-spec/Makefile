# 
# Makefile for the VDM++ to UML mapper
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

DEPDIR= ../dep-spec
ASTDIR= ../ast
LN = ln -sv 
LATEXFILE = mapper
SL_LATEXFILE = $(LATEXFILE:%=%-vdmsl)
PP_LATEXFILE = $(LATEXFILE:%=%-vdmpp)
LATEX = latex
RM = rm -f
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

# Modules written as mixed specifications
MIXED = common_as.vdm rep.vdm contextinfo.vdm

# Specific modules for the UML mapper in VDM++
PPSPECS = dep.vdm auml.vdm build.vdm error.vdm ast-merge.vdm

# Modules require-ring preprocessing
PPMIXED = $(MIXED:%.vdm=%.vdmpp)

#PPALL = $(PPSPECS) $(EQUAL) $(PPMIXED) asval.vdm
PPALL = $(PPSPECS) $(EQUAL) $(PPMIXED)

################################################################################
# 
# Main targets for the UML mapper
#

.PHONY: default
default: 
	echo Use make PP dvi all
	exit 1;

all: PP dvi

PP: .vdmde-uml-pp

dot: .vdmde-uml-pp

dvi: $(PP_LATEXFILE).dvi

# Make documentation  in ps format:
PPPS: $(PP_LATEXFILE).ps 

#
################################################################################


m4pt: argfile $(PPALL)
	/home/storm/toolbox/SunOS4/vdmde -ia ./argfile $(PPALL)
	rm argfile

argfile:
	echo "BUILD\`BuildUmlModel(ASVAL\`ast)" > argfile


linkall link: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;

vpath %.vdm $(ASTDIR) $(DEPDIR)

asval.vdm: mkasval modulehead
	mkasval

LATEX_PPSPECS = $(PPSPECS:%=%.tex) $(PPMIXED:%=%.tex) 

# Latex generation command
VDMDE = vdmde -lrN

# Test coverage information file 
RTINFO = 

$(LATEX_PPSPECS): %.tex: % $(RTINFO)
	 $(VDMDE) $<

$(RTINFO):
	@echo "Make link to $(RTINFO) for test coverage information"

# generating PP document as dvi - file
$(PP_LATEXFILE).dvi: $(PP_LATEXFILE).tex $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

$(PP_LATEXFILE).ps : $(PP_LATEXFILE).dvi
	dvips $(PP_LATEXFILE) -o

#
# Make .vdmde-pp for testing the PP specification
#

USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-uml-pp: $(PPALL)
	$(READ_CMD) $^ > $@


#
# VDM++ sources
#

$(PPMIXED): %.vdmpp: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMPP $< >$@	
	chmod a-w $@

$(PP_LATEXFILE).tex: %-vdmpp.tex: %.tex
	-rm -f $@
	$(IFDEFFILT) -DVDMPP $< >$@	
	chmod a-w $@

RMEXT = aux log idx ind blg ps dvi bbl ilg toc tex

clean:
	$(RM) $(LATEX_PPSPECS) 
	$(RM) *.vdmsl *.vdmpp $(DOTPP)
	$(RM) $(RMEXT:%=$(PP_LATEXFILE).%)





