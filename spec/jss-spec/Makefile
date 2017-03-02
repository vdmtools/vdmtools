JSSDIR= jss-spec
VDMDE = vdmde
ASTDIR= ../ast
CGDIR= ../cg-spec
LN = ln -s 

LATEXFILE = jssspec
LATEX = latex
RM = rm -f
#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

SPEC = jssdef.vdm jssstmt.vdm jssexpr.vdm jssenv.vdm jssaux.vdm jsserr.vdm \
       jssrep.vdm jssvcm1.vdm javaerr.vdm \
       jsserrmsg.vdm

PREPROC = common_as.vdm rep.vdm mod_cppast.vdm contextinfo.vdm util.vdm
PPMIXED =  $(PREPROC:%.vdm=%.vdmpp)
SPECALL = $(SPEC) $(PPMIXED)

################################################################################
# 
# Main targets for Java type checker
#

.PHONY: default
default: 
	echo Use make JSS dvi all
	exit 1;

# Preprocessing vdm file:
JSS: .vdmde-jss

# Make documentation (.tex files) in dvi format:
all: JSS dvi

dvi: $(LATEXFILE).dvi 

# Make documentation  in ps format:
JSSPS: $(LATEXFILE).ps 

$(PPMIXED): %.vdmpp: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMPP -DJSS $< >$@	
	chmod a-w $@



dot: .vdmde-jss
USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-jss: $(SPECALL)
	$(READ_CMD) $^ > $@

vpath %.vdm $(ASTDIR)
vpath %.vdmpp $(ASTDIR)
#vpath mod_cppast.% $(CGDIR)
vpath mod_cppast.vdm $(CGDIR)
vpath %.txt ../../code/errmsg
#
# Link
#

link: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;



#
# Documentation
#

%.ps : %.fig
	fig2dev -Lps $< $@ 

LATEX_JSSSPECS = $(SPECALL:%=%.tex) 
# Latex generation command
VDMDE = vdmde -lrN

# Test coverage information file 
RTINFO = # rtinfo.ast

%.tex: %
	$(VDMDE) $<

$(RTINFO):
	@echo "Make link to $(RTINFO) for test coverage information"

# generating JSS document as dvi - file
$(LATEXFILE).dvi: $(LATEXFILE).tex $(LATEX_JSSSPECS) 
	$(LATEX) $<
	bibtex $(<:.tex=)
	makeindex $(<:.tex=.idx)
	$(LATEX) $<
	$(LATEX) $<

# generating document as postscript file
$(LATEXFILE).ps : $(LATEXFILE).dvi
	dvips $(LATEXFILE) -o

#
# Generation of help texts
#
jsserrmsg.vdm: jsserrmsg.txt
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

RMEXT = aux log idx ind blg ps dvi bbl ilg toc aux
GENRMEXT = $(RMEXT) tex

clean:
	echo $(LATEX_JSSSPECS)
	$(RM) $(GENRMEXT:%=$(LATEX_JSSSPECS).%)
	$(RM) *.vdmsl *.vdmpp $(FIGPS) *.aux
	$(RM) $(RMEXT:%=$(LATEXFILE).%)
