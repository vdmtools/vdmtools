ASTDIR= ../ast

LN = ln -s 
LATEXFILE = trans
PDFFILE = trans
TRANS_LATEXFILE = $(LATEXFILE:%=%-vdm)
LATEX = latex
PDFLATEX = pdflatex
MAKEINDEX = makeindex
BIBTEX = bibtex
RM = rm -f
IFDEFFILT?=filepp -w -lc \\\\\\

TRANSSPECS = applyExprXform.vdm applyStmtXform.vdm applyXforms.vdm \
        findExprXform.vdm  findStmtXform.vdm  findXforms.vdm  \
        precedence.vdm     strip.vdm          stripBrackets.vdm \
        xforms.vdm

PREPROC = common_as.vdm rep.vdm contextinfo.vdm
PPMIXED =  $(PREPROC:%.vdm=%.vdmpp)

TRANSALL = $(TRANSSPECS) $(AST) $(PPMIXED)

################################################################################
# 
# Main targets
#
# Make links in current directory to specifications in sub-dirs and
# create script file .vdmde:

.PHONY: default
default: 
	echo Use make dvi all
	exit 1;

link linkall: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;

all: TRANS dvi pdf

# Preprocessing vdm files:
TRANS:   .vdmde-trans-pp

dot:  .vdmde-trans-pp

dvi: $(TRANS_LATEXFILE).dvi

pdf: $(PDFFILE).pdf

# Make documentation  in ps format:
TRANSPS: $(TRANS_LATEXFILE).ps 

$(PPMIXED): %.vdmpp: %.vdm
	-rm -f $@
#	$(IFDEFFILT) -DVDMPP -DTPINFO -DTRANS $< >$@	
	$(IFDEFFILT) -DVDMPP -DTRANS $< >$@	
	chmod a-w $@

#
################################################################################
vpath %.vdm $(ASTDIR)
vpath %.vdmpp $(ASTDIR)

LATEX_TRANSTRSPECS = $(TRANSTRSPECS:%=%.tex)
# Latex generation command
VDMDE = vdmde -lrN

# Test coverage information file 
RTINFO = # rtinfo.ast

$(LATEX_EQUAL_SPECS) $(LATEX_TRANSTRSPECS): %.tex: % $(RTINFO)
	 $(VDMDE) $<
	 cat $<.tex | sed -e's/\\color{not_covered}/\\color{not_covered}\\index{Uncovered Construct}/g' > $<.tmp
	 mv -f $<.tmp $<.tex

# generating TRANS document as dvi - file
%.dvi: $(LATEXFILE).tex $(LATEX_TRANSTRSPECS) $(LATEX_EQUAL_SPECS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

$(LATEXFILE).ps : $(LATEXFILE).dvi
	dvips $(LATEXFILE) -o

$(TRANS_LATEXFILE).tex: %-vdm.tex: %.tex
	-rm -f $@
#	$(IFDEFFILT) -DVDMPP -DTPINFO $< >$@	
	$(IFDEFFILT) -DVDMPP $< >$@	
	chmod a-w $@

$(PDFFILE).pdf: $(LATEXFILE).tex
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd| cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd|sed -e "s/ //g")
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-trans-pp: $(TRANSALL)
	$(READ_CMD) $^ > $@

.vdmde-win-trans-pp: .vdmde-trans-pp
	cat $^ | sed -e "s /home //hermes g" > $@

RMEXT = aux log idx ind blg ps dvi bbl ilg toc tex

clean:
	$(RM) $(LATEX_SLSPECS) $(LATEX_TRANSTRSPECS) $(LATEX_EQUAL_SPECS) 
	$(RM) *.vdmsl *.vdmpp *.vdmrt $(DOTSL) $(DOTPP)
	$(RM) $(RMEXT:%=$(SL_LATEXFILE).%) $(RMEXT:%=$(TRANS_LATEXFILE).%)
