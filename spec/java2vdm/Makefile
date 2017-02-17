STDIR= ../ast
CGDIR= ../cg-spec
ASTDIR= ../ast
JSSDIR= ../jss-spec
J2VTRDIR = ../transforms-spec

LN = ln -s 
LATEXFILE = java2vdm++
PDFFILE = java2vdm++
J2V_LATEXFILE = $(LATEXFILE:%=%-vdm)
LATEX = latex
PDFLATEX = pdflatex
MAKEINDEX = makeindex
BIBTEX = bibtex
RM = rm -f
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

# VDM-SL Modules for the Java to VDM++ Translator
J2VSPECS = j2v.vdm j2vaux.vdm j2vbvdm.vdm j2vclass.vdm j2vop.vdm j2vtype.vdm \
           j2vexpr.vdm j2vstmt.vdm j2vns.vdm j2venv.vdm \
           j2verrmsg.vdm jsserrmsg.vdm j2verr.vdm jssrep.vdm

AST=
#mangle.vdm

JSS=jssdef.vdm  jssstmt.vdm jssexpr.vdm \
    jssenv.vdm  jssaux.vdm  jsserr.vdm javaerr.vdm \
    jssjava.vdm jssrep.vdm  jssvcm1.vdm 
#jsserrmsg.vdm

J2VTR = applyExprXform.vdm applyStmtXform.vdm applyXforms.vdm \
        findExprXform.vdm  findStmtXform.vdm  findXforms.vdm  \
        precedence.vdm     strip.vdm          stripBrackets.vdm \
        xforms.vdm

PREPROC = common_as.vdm rep.vdm mod_cppast.vdm contextinfo.vdm mangle.vdm util.vdm
PPMIXED =  $(PREPROC:%.vdm=%.vdmpp)

J2VALL = $(J2VSPECS) $(JSS) $(J2VTR) $(AST) $(PPMIXED)

################################################################################
# 
# Main targets for the Java to VDM++ Translator
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

all: J2V dvi pdf

# Preprocessing vdm files:
J2V:   .vdmde-j2v-pp

dot:  .vdmde-j2v-pp

dvi: $(J2V_LATEXFILE).dvi

pdf: $(PDFFILE).pdf

# Make documentation  in ps format:
J2VPS: $(J2V_LATEXFILE).ps 

$(PPMIXED): %.vdmpp: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMPP -DJ2V -DJSS $< >$@	
	chmod a-w $@

#
################################################################################
vpath %.vdm $(JSSDIR)
vpath %.vdm $(ASTDIR)
vpath %.vdmpp $(ASTDIR)
vpath mod_cppast.vdm $(CGDIR)
vpath %.txt ../../code/errmsg
vpath %.txt $(JSSDIR)
vpath %.txt ./
vpath %.tex $(ASTDIR)
vpath %.vdm $(J2VTRDIR)

LATEX_J2VSPECS = $(J2VSPECS:%=%.tex)
# Latex generation command
VDMDE = vdmde -lrN

# Test coverage information file 
RTINFO = # rtinfo.ast

$(LATEX_EQUAL_SPECS) $(LATEX_J2VSPECS): %.tex: % $(RTINFO)
	 $(VDMDE) $<
	 cat $<.tex | sed -e's/\\color{not_covered}/\\color{not_covered}\\index{Uncovered Construct}/g' > $<.tmp
	 mv -f $<.tmp $<.tex

# generating J2V document as dvi - file
%.dvi: $(LATEXFILE).tex $(LATEX_J2VSPECS) $(LATEX_EQUAL_SPECS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

$(LATEXFILE).ps : $(LATEXFILE).dvi
	dvips $(LATEXFILE) -o

$(J2V_LATEXFILE).tex: %-vdm.tex: %.tex
	-rm -f $@
	$(IFDEFFILT) -DVDMPP -DJSS $< >$@	
	chmod a-w $@

$(PDFFILE).pdf: $(LATEXFILE).tex
	$(PDFLATEX) $<
	$(BIBTEX) $(<:.tex=)
	$(MAKEINDEX) -c $(<:.tex=.idx)
	$(PDFLATEX) $<
	$(PDFLATEX) $<

#
# Generation of help texts
#
jsserrmsg.vdm: jsserrmsg.txt
	$(JSSDIR)/translateErr.pl vdm $<

j2verrmsg.vdm: j2verrmsg.txt
	$(<D)/translateErr.pl vdm $<

USER=`whoami`
#MPWD=$(shell /bin/pwd|sed -e "s/ //g")
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-j2v-pp: $(J2VALL)
	$(READ_CMD) $^ > $@

.vdmde-win-j2v-pp: .vdmde-j2v-pp
	cat $^ | sed -e "s /home //hermes g" > $@

RMEXT = aux log idx ind blg ps dvi bbl ilg toc tex

clean:
	$(RM) $(LATEX_SLSPECS) $(LATEX_J2VSPECS) $(LATEX_EQUAL_SPECS) 
	$(RM) *.vdmsl *.vdmpp *.vdmrt $(DOTSL) $(DOTPP)
	$(RM) $(RMEXT:%=$(SL_LATEXFILE).%) $(RMEXT:%=$(J2V_LATEXFILE).%)
