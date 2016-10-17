ASTDIR= ../ast
LATEX = latex
RM = rm -f
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

LATEXFILE = astdoc
PP_LATEXFILE = $(LATEXFILE:%=%-vdmpp)

# Modules common for SL and PP but mixed specifications
MIXED = common_as.vdm rep.vdm contextinfo.vdm 

# Modules equal for SL and PP
EQUAL = test.vdm stmtxform.vdm xforms.vdm exprxform.vdm strip.vdm

# Modules require-ring preprocessing
PPMIXED = $(MIXED:%.vdm=%.vdmpp)

PPALL = $(PPSPECS) $(EQUAL) $(PPMIXED) 


# Preprocessing vdm files:
PP: $(PPALL) dot

# Make documentation (.tex files) in dvi format:
PPTEX all: $(PP_LATEXFILE).dvi

# Make documentation  in ps format:
PPPS: $(PP_LATEXFILE).ps 

dot:   .vdmde-j2vtf-pp

vpath %.vdm $(ASTDIR)
vpath %.tex $(ASTDIR)

LATEX_SLSPECS = $(SLSPECS:%=%.tex) $(SLMIXED:%=%.tex) 
LATEX_PPSPECS = $(PPSPECS:%=%.tex) $(PPMIXED:%=%.tex) 
LATEX_EQUAL_SPECS = $(EQUAL:%=%.tex)

FIG = rep.fig
FIGPS = $(FIG:.fig=.ps)

%.ps : %.fig
	fig2dev -Lps $< $@ 


# Latex generation command
VDMDE = vdmde -lrN


$(LATEX_SLSPECS) $(LATEX_EQUAL_SPECS) $(LATEX_PPSPECS): %.tex: % $(RTINFO)
	 $(VDMDE) $<


# generating PP document as dvi - file
$(PP_LATEXFILE).dvi: $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) $(PP_LATEXFILE).tex $(FIGPS)
	$(LATEX) $(PP_LATEXFILE)
	makeindex $(PP_LATEXFILE).idx
	$(LATEX) $(PP_LATEXFILE)

# generating document as postscript file
$(PP_LATEXFILE).ps : $(PP_LATEXFILE).dvi
	dvips $(PP_LATEXFILE) -o



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

USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-pog-pp: $(PPALL)
	$(READ_CMD) $^ > $@

RMEXT = aux log idx ind blg ps dvi bbl ilg toc tex

clean:
	$(RM) $(LATEX_PPSPECS) $(LATEX_EQUAL_SPECS) 
	$(RM) *.vdmpp $(DOTPP)
	$(RM) $(RMEXT:%=$(PP_LATEXFILE).%)

#USER=`whoami`
#MPWD=$(shell /bin/pwd|sed -e "s .*$(USER) $(HOME) g")
#READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-j2vtf-pp: $(PPALL)
	$(READ_CMD) $^ > $@
