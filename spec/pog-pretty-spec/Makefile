POG_AUX = ../pog-aux-spec
POG = ../pog-spec
ASTDIR = ../ast
SSDIR = ../ss-spec
STDLIBDIR = ../stdlib-spec
LN = ln -s 
IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

# This is use for testdir -s
.PHONY:	SL
# modules of dynamic semantic

# Modules that need preprocessing because they are mixed SL and PP
MIXED = common_as.vdm rep.vdm contextinfo.vdm \
	env.vdm type.vdm vcm.vdm err.vdm util.vdm\
        pogTP.vdm \
        pp-mediator.vdm pp-traverse-def.vdm \
        pp-target-def.vdm \
	pp-target-patterns.vdm \
	pp-traverse-expr.vdm \
	pp-traverse-patterns.vdm \
        spec-ast.vdm

# Modules that are used in both PP and SL, but needs no preprocessing.
EQUAL = errmsg.vdm

# Modules that are specific for and only used in SL
SLSPEC = pp-target-expr.vdm \
	pp-target-type.vdm \
 	pp-target-po.vdm \
	pp-traverse-type.vdm \
	pp-traverse-po.vdm

PPSPEC_PREPRO = mangle.vdm

SLMIXED   = $(MIXED:%.vdm=%.vdmsl)
PPMIXED   = $(MIXED:%.vdm=%.vdmpp) $(PPSPEC_PREPRO:%.vdm=%.vdmpp)
RTMIXED   = $(MIXED:%.vdm=%.vdmrt) $(PPSPEC_PREPRO:%.vdm=%.vdmrt)

SLALL   = $(SLSPEC) $(EQUAL) $(SLMIXED)
PPALL   = $(SLSPEC) $(EQUAL) $(PPMIXED)
RTALL   = $(SLSPEC) $(EQUAL) $(RTMIXED)

errmsg.vdm: errmsg.txt
	$(<D)/translateErr.pl vdm $<

dummy: 
	echo Specify a make target like SL  all dot 

dot: SL PP RT

SL:   .vdmde-pog-pretty-sl
PP:   .vdmde-pog-pretty-pp
RT:   .vdmde-pog-pretty-rt

all: dot 

# Use vpath to find source files dynamically instead of having to 
# create links.

#vpath %.vdmsl $(POG_AUX) $(POG)
vpath %.vdm $(ASTDIR) $(STDLIBDIR) $(POG_AUX) $(POG) $(SSDIR)
vpath %.fig $(ASTDIR)
vpath %.vdm $(SSDIR)
vpath %.txt ../../code/errmsg



link linkall: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;

SLTEX_ALL = $(SLALL:%=%.tex)

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

.vdmde-pog-pretty-sl: $(SLALL)
	$(READ_CMD) $^ > $@

%.vdmsl: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMSL $< >$@
	chmod a-w $@

.vdmde-pog-pretty-pp: $(PPALL)
	$(READ_CMD) $^ > $@

%.vdmpp: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVDMPP $< >$@
	chmod a-w $@

.vdmde-pog-pretty-rt: $(RTALL)
	$(READ_CMD) $^ > $@

%.vdmrt: %.vdm
	-rm -f $@
	$(IFDEFFILT) -DVICE -DVDMPP $< >$@
	chmod a-w $@

clean:
	rm -f *.vdmsl
	rm -f *.vdmpp
	rm -f *.vdmrt


