# WHAT
#    Makefile for the code generated VDM++ sort example.
# ID
#    $Id: Makefile,v 1.24 2005/12/21 06:41:45 vdmtools Exp $
# PROJECT
#    Toolbox
# COPYRIGHT
#    (C) 2016 Kyushu University
#
# REMEMBER to change the variable TBDIR to fit your directory structure.
#

OSTYPE=$(shell uname)

GCC     = $(CC)
CXX     = $(CCC)
TBDIR   = ../..
VPPDE   = $(TBDIR)/bin/vppde

INCL    = -I$(TBDIR)/cg/include

ifeq ($(strip $(OSTYPE)),Darwin)
OSV = $(shell uname -r)
OSMV = $(word 1, $(subst ., ,$(strip $(OSV))))
LION_OR_LATER=$(shell [ $(OSMV) -ge 11 ] && echo true)

CCPATH = /usr/bin/
ifeq ($(strip $(LION_OR_LATER)),true) # >= 10.7
CC      = $(CCPATH)clang
CCC     = $(CCPATH)clang++
else
ifeq ($(strip $(OSMV)),10) # 10.6
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
else
ifeq ($(strip $(OSMV)),9) # 10.5
CC      = $(CCPATH)gcc-4.2
CCC     = $(CCPATH)g++-4.2
else
ifeq ($(strip $(OSMV)),8) # 10.4
CC      = $(CCPATH)gcc-4.0
CCC     = $(CCPATH)g++-4.0
else
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
endif
endif
endif
endif
LIB     = -L$(TBDIR)/cg/lib -lCG -lvdm -lm -liconv
endif

ifeq ($(strip $(OSTYPE)),Linux)
CCPATH = /usr/bin/
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
LIB     = -L$(TBDIR)/cg/lib -lCG -lvdm -lm
endif

ifeq ($(strip $(OSTYPE)),SunOS)
CCPATH = /usr/sfw/bin/
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
LIB     = -L$(TBDIR)/cg/lib -L../../lib -lCG -lvdm -lm
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
CCPATH = /usr/bin/
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
LIB     = -L$(TBDIR)/cg/lib -lCG -lvdm -lm -L/usr/local/lib -liconv
endif

CFLAGS	= -g $(INCL)
CCFLAGS	= $(CFLAGS)
CXXFLAGS= $(CCFLAGS)

OSTYPE2=$(word 1, $(subst _, ,$(strip $(OSTYPE))))
ifeq ($(strip $(OSTYPE2)),CYGWIN)
all: sort_pp.exe SortMain
else
all: sort_pp SortMain
endif

ifeq ($(strip $(OSTYPE2)),CYGWIN)
CCPATH = /usr/bin/
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
LIB     = -L$(TBDIR)/cg/lib -lCG -lvdm -liconv
endif

sort_pp.exe:
	make -f Makefile.winnt

ALLFILES = DoSort ExplSort ImplSort MergeSort SortMachine Sorter

GENCCFILES = DoSort.cc DoSort.h DoSort_anonym.cc DoSort_anonym.h \
             ExplSort.cc ExplSort.h ExplSort_anonym.cc ExplSort_anonym.h \
             ImplSort.cc ImplSort.h ImplSort_anonym.cc ImplSort_anonym.h \
             MergeSort.cc MergeSort.h MergeSort_anonym.cc MergeSort_anonym.h \
             Sorter.cc Sorter.h Sorter_anonym.cc Sorter_anonym.h \
	     SortMachine.cc SortMachine.h SortMachine_anonym.cc SortMachine_anonym.h \
	     CGBase.cc CGBase.h

GENJAVAFILES = $(ALLFILES:%=%.java)

sort_pp : sort_pp.o $(ALLFILES:%=%.o) CGBase.o
	$(CCC) -o sort_pp  sort_pp.o $(ALLFILES:%=%.o) CGBase.o $(LIB)

DoSort.o: DoSort.cc DoSort.h DoSort_anonym.h DoSort_anonym.cc \
        CGBase.h Sorter.h DoSort_userdef.h
ExplSort.o: ExplSort.cc ExplSort.h  ExplSort_anonym.h \
        ExplSort_anonym.cc CGBase.h Sorter.h ExplSort_userdef.h
ImplSort.o: ImplSort.cc ImplSort.h ImplSort_anonym.h \
        ImplSort_anonym.cc CGBase.h Sorter.h ImplSort_userimpl.cc \
        ImplSort_userdef.h
MergeSort.o: MergeSort.cc MergeSort.h  MergeSort_anonym.h \
        MergeSort_anonym.cc CGBase.h Sorter.h MergeSort_userdef.h
SortMachine.o: SortMachine.cc SortMachine.h CGBase.h Sorter.h \
        MergeSort.h 
Sorter.o: Sorter.cc Sorter.h CGBase.h
CGBase.o: CGBase.cc CGBase.h
sort_pp.o: $(GENCCFILES)
SortMain.class: SortMain.java $(GENJAVAFILES)
SortMain: SortMain.class $(GENJAVAFILES:%.java=%.class)

ifeq ($(strip $(OSTYPE)),Darwin)
%.class : %.java
	javac -J-Dfile.encoding=UTF-8 -encoding UTF8 -classpath ../../javacg/VDM.jar:. $<
else
ifeq ($(strip $(OSTYPE2)),CYGWIN)
%.class : %.java
	javac -classpath "../../javacg/VDM.jar;." $<
else
%.class : %.java
	javac -classpath ../../javacg/VDM.jar:. $<
endif
endif

SPECFILES = dosort.vpp explsort.vpp implsort.vpp mergesort.vpp \
        sorter.vpp sortmachine.vpp

$(GENCCFILES): $(SPECFILES)
	$(VPPDE) -c -P $^

$(GENJAVAFILES): $(SPECFILES)
	$(VPPDE) -j -P $^

###########################################################
#### Generation of postscript of the sort.tex document ####
###########################################################

VDMLOOP = vdmloop

GENFILES = sort.aux sort.log sort.ind sort.idx sort.ilg vdm.tc

init:
	cp mergesort.init mergesort.vpp

vdm.tc:
	cd test; $(VDMLOOP)
	cp -f test/$@ .

%.tex: $(SPECFILES) vdm.tc
	vppde -lrNn $(SPECFILES)

sort.ps: $(SPECFILES).tex
	latex sort.tex
	makeindex sort
	latex sort.tex
	latex sort.tex
	dvips sort.dvi -o

clean: 
	rm -f *.o sort_pp
	rm -f *.class *.java.bak
	rm -f sort.ps sort.dvi
	rm -f $(SPECFILES:%=%.tex)
	rm -f $(SPECFILES:%=%.aux)
	rm -f $(GENFILES)
	rm -f $(GENCCFILES)
	rm -f $(GENJAVAFILES)
	rm -f *.obj *.cpp *.exe 







