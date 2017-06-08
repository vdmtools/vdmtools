all \
     io.vdm io.vpp ioflat.vdm \
     math.vdm math.vpp mathflat.vdm \
     VDMUtil.vdm VDMUtil.vpp VDMUtilflat.vdm:
	./gen-libs.sh

#IFDEFFILT?=../../code/make_tools/ifdeffilt
IFDEFFILT?=filepp -w -lc \\\\\\

PPALL = io.vpp math.vpp VDMUtil.vpp \
	$(PPCODEDIR)/IO_userimpl.cc $(PPCODEDIR)/MATH_userimpl.cc $(PPCODEDIR)/VDMUtil_userimpl.cc \
	$(PPCODEDIR)/IO_userimpl.cpp $(PPCODEDIR)/MATH_userimpl.cpp $(PPCODEDIR)/VDMUtil_userimpl.cpp
SLALL = io.vdm math.vdm VDMUtil.vdm \
	$(SLCODEDIR)/IO_userimpl.cc $(SLCODEDIR)/MATH_userimpl.cc $(SLCODEDIR)/VDMUtil_userimpl.cc \
	$(SLCODEDIR)/IO_userimpl.cpp $(SLCODEDIR)/MATH_userimpl.cpp $(SLCODEDIR)/VDMUtil_userimpl.cpp

PPCODEDIR=code/pp
SLCODEDIR=code/sl

$(PPCODEDIR)/%.cc: code/%.cc
#	$(IFDEFFILT) -DVDMPP $< > $@
	perl -nle 's/#include/edulcni/; print;' $< > tmp
	$(IFDEFFILT) -DVDMPP tmp | \
	perl -nle 's/edulcni/#include/; print;' > $@
	rm -rf tmp

$(PPCODEDIR)/%.cpp: code/%.cc
	ln -s $(notdir $<) $@

$(SLCODEDIR)/%.cpp: code/%.cc
	ln -s $(notdir $<) $@

$(SLCODEDIR)/%.cc: code/%.cc
#	$(IFDEFFILT) -DVDMSL $< > $@
	perl -nle 's/#include/edulcni/; print;' $< > tmp
	$(IFDEFFILT) -DVDMSL tmp | \
	perl -nle 's/edulcni/#include/; print;' > $@
	rm -rf tmp

clean:
	rm -rf math.vdm math.vpp mathflat.vdm io.vdm io.vpp ioflat.vdm
	rm -rf VDMUtil.vdm VDMUtil.vpp VDMUtilflat.vdm VDMByteUtil.vdm VDMByteUtil.vpp VDMByteUtilflat.vdm
	rm -f $(PPCODEDIR)/*.cc $(PPCODEDIR)/*.cpp
	rm -f $(SLCODEDIR)/*.cc $(SLCODEDIR)/*.cpp

SL:   .vdmde-stdlib-sl 
PP:   .vdmde-stdlib-pp

USER=`whoami`
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-stdlib-sl: $(SLALL)
	$(READ_CMD) $^ > $@

.vdmde-stdlib-pp: $(PPALL)
	$(READ_CMD) $^ > $@  
