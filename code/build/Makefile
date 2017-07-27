# for VDM PRODUCTS
# Copyright (C) Kyushu University

# RCS identification
#
# $Id: Makefile,v 1.707 2006/09/23 11:13:30 vdmtools Exp $

#############################################################################
# TOOLBOX MODULES/DIRECTORIES
#############################################################################

# Never change BASEDIR
# Instead do make linkALL BASEDIR=<path to code dir>
#BASEDIR = ../code
#BUILD   = $(BASEDIR)/build
#DIVDIR  = $(BASEDIR)/diverse
#EVALDIR = $(BASEDIR)/eval
#SFDIR   = $(BASEDIR)/specfile
#CGDIR   = $(BASEDIR)/cg
#LATEXDIR= $(BASEDIR)/latex
#LIBDIR  = $(BASEDIR)/lib
#PARSDIR = $(BASEDIR)/parser
#RECODIR = $(BASEDIR)/recover
#SSDIR   = $(BASEDIR)/statsem
# Module exc_emul is no longer used. 
#RANDIR  = $(BASEDIR)/random
#GUIDIR  = $(BASEDIR)/gui
#UTILSDIR= $(BASEDIR)/utils
#FLEXDIR = $(BASEDIR)/flexlm
#MGRDIR  = $(BASEDIR)/specman
#MGRCODE = $(MGRDIR)/code
#GENMGRC = $(MGRDIR)/gencode

# UML STUFF

#INHTREE = $(BASEDIR)/inhtree

#UMLDIR  = $(BASEDIR)/uml
#UMLCODE = $(UMLDIR)/code
#GENUMLC = $(UMLDIR)/gencode
#DEPDIR  = $(BASEDIR)/dep
#DEPCODE = $(DEPDIR)/code
#GENDEPC = $(DEPDIR)/gencode
#ASREPDIR   = $(BASEDIR)/as-rep
#MAKE_TOOLS = $(BASEDIR)/make_tools
#PORTDIR = $(BASEDIR)/port
#WIN32DIR = $(BASEDIR)/win32
#CORBACODE = $(BASEDIR)/api/corba

# A file defining a mapping between module names and an enumeration offset
# used when patching a code generated header file:
#TAG-OFFSETS = tag_offsets

.PRECIOUS: %-sl.cc %-pp.cc

# New change: investigate 
# BASEDIR = ../code
# VPATH=$(DIVDIR):$(EVALDIR):$(CGDIR):$(LIBDIR):$(PARSDIR):$(RECODIR):$(SSDIR):$(EXCDIR):$(RANDIR)

ifeq ($(strip $(OSTYPE)),cygwin32)
override OSTYPE:=win32
endif

#############################################################################
# Set OSTYPE correct for the different flavors of Linux
#############################################################################
ifeq ($(strip $(OSTYPE)),linux)
	OSTYPE = Linux
endif
REMOVE_ME_IF_YOU_DARE := 1 // Needed to make the following test work.
ifeq ($(strip $(OSTYPE)),linux-gnu)
	OSTYPE = Linux
endif

#############################################################################
# LNCONf setup
#############################################################################
ifeq ($(strip $(OSTYPE)),win32)
#LNCONF = lnconf
LNCONF = /usr/bin/perl $(VPATH)/lnconf
LNCONF_OPT = -c -w
else
LNCONF = ./lnconf
LNCONF_OPT = -c
endif
#DUMMY := $(shell echo "NOTE: CG is disabled for vdmpp . " >/dev/tty)
INIT:= $(shell OSTYPE=$(OSTYPE) $(LNCONF) $(LNCONF_OPT) &)


#############################################################################
# REALOS setup
#############################################################################

UNAME = $(shell uname)
REALOS = $(word 1, $(subst -, ,$(strip $(UNAME))))

#############################################################################
# DEPENDENCY RULES
#############################################################################

LIB_OUTPUT_OPTION = -o $%
ifeq ($(strip $(OSTYPE)),win32)
WIN32_PROFILE = 
endif

%.o: %.cc
	$(COMPILE.cc) $(SRCNAME)$< $(OUTPUT_OPTION)

%.o: %.c
	$(COMPILE.c) $(SRCNAME)$< $(OUTPUT_OPTION)

ifdef WIN32_PROFILE
(%.o): %.cc
	$(COMPILE.cc) $(SRCNAME)$< $(LIB_OUTPUT_OPTION)
	@if [ -f $@ ]; then \
	echo "$(AR) /OUT:$@ $% $@ "; \
	$(AR) /OUT:$@ $% $@ ;\
	else \
	$(AR) /OUT:$@ $% ;\
	echo "$(AR) /OUT:$@ $%"; \
	fi
	$(RM) $%
else
(%.o): %.cc
	$(COMPILE.cc) $(SRCNAME)$< $(LIB_OUTPUT_OPTION)
	$(AR) $(ARFLAGS) $@ $% 
	$(RM) $%
endif

ifndef OSTYPE 
$(error "Error: variable OSTYPE is not set or exported.")
endif

# -O2

# Default fixparser. Created to fix long jump tables for G++ on SGI
# FIXPARSER = cat 
# Default option for making tar follow symlinks. 
TARFOLLOWLINKS=h

RANLIB=@echo ranlib not needed

#############################################################################
# PLATFORM SETUP
#############################################################################
USE_CORBA_API ?= 1

ifndef JDKHOME
ifdef JDK_HOME
  JDKHOME = $(JDK_HOME)
endif
ifdef JAVA_HOME
  JDKHOME = $(JAVA_HOME)
endif
endif

dummy := $(foreach V, OSTYPE REALOS USE_CORBA_API, \
  $(shell echo make is using $V=$($V) >/dev/tty) \
)
ifeq ($(strip $(OSTYPE)),Darwin)
dummy := $(shell echo make is using MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >/dev/tty)
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
else
  LIB_DL = -ldl
endif

#OPTIMIZE += -O
#DEBUG =-g

VDMSL=-DVDMSL
VDMPP=-DVDMPP
VDMRT=-DVICE -DVDMPP

PREPROCESS = $(GCC) -P -E -C
ifeq ($(strip $(OSTYPE)),Darwin)
PREPROCESS += -D__darwin__
endif

# -Wno-return-type is needed for egcs. See http://egcs.cygnus.com/caveats.html
ifeq ($(strip $(OSTYPE)),Darwin)
WARNINGS = -Wall -Wno-unused -Wno-deprecated
else
WARNINGS = -Wall -Wno-unused
endif

DEFINES = $(GLOBALDEFINES) $(ARCH_DEFINES)

GLOBALDEFINES = # -DSHOW_MALLOC_STATS -DVDM_FINGERPRINT

VDMTOOLSCFLAGS = $(ARCH) $(WARNINGS) $(PROFILING) $(OPTIMIZE) $(DEBUG) $(WCHAR) \
	    -I. $(INCDIR)

#-rdynamic 
#LNKOPT = $(STATIC) $(DBMALLOC) -L. $(PROFILING) $(GNULIBPATH) \
#	$(LIB_DL) $(LINK_DEBUG) $(CORBALIB) #-v -Wl,-verbose
LNKOPTX = $(STATIC) $(DBMALLOC) -L. $(PROFILING) $(GNULIBPATH) \
	$(LINK_DEBUG) $(CORBALIB) #-v -Wl,-verbose
LNKOPT =$(strip $(LNKOPTX))
GUILNKOPT = $(LNKOPT) $(QTLIBFLAG)

ifeq ($(strip $(OSTYPE)),win32)
INCDIR = $(SHADOWINC)
else
ifeq ($(strip $(OSTYPE)),SunOS)
ifneq ($(strip $(FLEXROOT)),/usr/sfw)
INCDIR = -I $(FLEXROOT)/include
endif
else
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.8)
INCDIR = -I $(FLEXROOT)/include
endif
else
ifneq ($(strip $(FLEXROOT)),/usr)
INCDIR = -I $(FLEXROOT)/include
endif
endif
endif
endif

ifeq ($(strip $(YACC)),yacc)
#YACC = bison-1.28
YACC = bison-2.3
endif

BISONVERSIONSTR = $(shell $(YACC) --version)
BISONVERSION = $(word 4, $(strip $(BISONVERSIONSTR)))

bison:
	echo $(BISONVERSION)

ifeq ($(strip $(REALOS)),CYGWIN_NT)
BISON2=1
else
ifeq ($(strip $(BISONVERSION)),1.28)
else
BISON2=1
endif
endif

ifdef BISON2
BISONFLAG =
else
BISONFLAG = -DUSE_BISON128
PREPROCESS += -DUSE_BISON128
endif

#YACCOPT = -d -v 
YACCOPT = -d -v -t
# add 't' for debugging opportunity, 'v' for verbose output file.
#FLEX     = $(FLEXROOT)/bin/flex -v -b -p -p -Cf -8
FLEX     = $(FLEXROOT)/bin/flex -v -b -p -f -B -8

RM      = rm -f
MV      = mv -f
CP      = cp -f

# LN      = ln -s
ifeq ($(strip $(OSTYPE)),win32)
LN      = $(CP)
else
LN      = ./sln -s
endif

PERL  =  perl
LIBM4   = -lvdm
PARSSL  = -lparser-sl 
PARSPP  = -lparser-pp
LIBGENSL= -lgen-sl
LIBGENPP= -lgen-pp
LIBSSSL	= -lss-sl
LIBSSPP = -lss-pp
LIBCGSL	= -lCG-sl
LIBCGPP	= -lCG-pp
LIBEVALSL = -leval-sl
LIBEVALPP = -leval-pp
LIBMGRSL = -lMGR-sl
LIBMGRPP = -lMGR-pp
LIBMATH = -lm

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
CXX     = $(CCC)
GCC     = $(CCPATH)gcc
else
ifneq ("$(wildcard /usr/bin/clang)","")
CC      = $(CCPATH)clang
CCC     = $(CCPATH)clang++
CXX     = $(CCC)
GCC     = $(CCPATH)clang
else
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++
CXX     = $(CCC)
GCC     = $(CCPATH)gcc
endif
endif
COMPILE_OPTION = true

# Notice: 
# PPOTYPE expands to $(VDMPP) when the target is x-pp.o
# SLOTYPE expands to $(VDMSL) when the target is x-sl.o
#

SLOTYPE = $(VDM$(patsubst %-sl.o,SL,$(filter %-sl.o, $@)))
PPOTYPE = $(VDM$(patsubst %-pp.o,PP,$(filter %-pp.o, $@)))
RTOTYPE = $(VDM$(patsubst %-rt.o,RT,$(filter %-rt.o, $@)))
PPSLFLAGO= $(PPOTYPE) $(SLOTYPE) $(RTOTYPE)

#Notice:
#PPFLAG: if the target archive has extension x-pp.a then PPFLAG 
#        expands to $(VDMPP)
#SLFLAG: if the target archive has extension x-sl.a then SLFLAG 
#        expands to $(VDMSL)

SLFLAG = $(VDM$(patsubst %-sl.a,SL,$@))
PPFLAG = $(VDM$(patsubst %-pp.a,PP,$@))
RTFLAG = $(VDM$(patsubst %-rt.a,RT,$@))
PPSLFLAGA = $(PPFLAG) $(SLFLAG) $(RTFLAG)

SLIFLAG   = $(VDM$(patsubst %-sl.i,SL,$@))
PPIFLAG   = $(VDM$(patsubst %-pp.i,PP,$@))
RTIFLAG   = $(VDM$(patsubst %-rt.i,RT,$@))
PPSLFLAGI = $(PPIFLAG) $(SLIFLAG) $(RTIFLAG)

# Notice:
# $(CFLAGS-$%) and $(CFLAGS-$@) is used for target specific flags, e.g., 
# with names like CFLAGS-vdmde$.o 
# $(CFLAGS-$%) is applicable when libX.a(x.o) is being build because $% 
# expands to x.o. $@ expands to libX.a in this case, so with the define
# of CFLAGS below it is possible to both have a library and object specific
# defines. 
# $(CFLAGS-$@) is applicable when x.o is being built. In this case $% is empty.

ifeq ($(strip $(OSTYPE)),win32)
EXCEPTIONFLAG =
else 
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.12)
EXCEPTIONFLAG = -fexceptions
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.11)
EXCEPTIONFLAG = -fexceptions
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.10)
EXCEPTIONFLAG = -fexceptions
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.9)
EXCEPTIONFLAG = -fexceptions
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.8)
EXCEPTIONFLAG = -fexceptions
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.7)
EXCEPTIONFLAG = -fexceptions
else
EXCEPTIONFLAG = -fexceptions -fnon-call-exceptions
endif
endif
endif
endif
endif
endif
else 
EXCEPTIONFLAG = -fexceptions -fnon-call-exceptions
endif
endif

CFLAGS = $(VDMTOOLSCFLAGS) $(DEFINES) $(CFLAGS-$%) $(CFLAGS-$@) $(PPSLFLAGA) $(PPSLFLAGO) $(PPSLFLAGI) $(BISONFLAG) \
         $(EXCEPTIONFLAG)

CCFLAGS   = $(strip $(CFLAGS))
CXXFLAGS  = $(strip $(CCFLAGS) $(TEMPLATE_FLAGS))

# When profiling with GPROF this can be usefull to add to Override.mak
# (with out #).

#CFLAGS-libvdm.a = -DUSE_OUR_OWN_OPERATOR_NEW

# Flags for the UML Tool. Below, links are made to the original .cc files

CFLAGS-libUML-pp.a += $(XMLINCL)
CFLAGS-libUML-rt.a += $(XMLINCL)

ifeq ($(strip $(OSTYPE)),win32)
CFLAGS-libUMLROSE-pp.a += $(XMLINCL) -D_USE_ROSE
CFLAGS-libUMLROSE-rt.a += $(XMLINCL) -D_USE_ROSE
endif

CFLAGS-libvdm_shared.a += -fPIC
CFLAGS-libCG_shared.a += -fPIC

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.12)
CFLAGS-libvdm_fat.a += -arch i386
CFLAGS-libCG_fat.a += -arch i386
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.11)
CFLAGS-libvdm_fat.a += -arch i386
CFLAGS-libCG_fat.a += -arch i386
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.10)
CFLAGS-libvdm_fat.a += -arch i386
CFLAGS-libCG_fat.a += -arch i386
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.9)
CFLAGS-libvdm_fat.a += -arch i386
CFLAGS-libCG_fat.a += -arch i386
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.8)
CFLAGS-libvdm_fat.a += -arch i386
CFLAGS-libCG_fat.a += -arch i386
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.7)
CFLAGS-libvdm_fat.a += -arch i386
CFLAGS-libCG_fat.a += -arch i386
else
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.6)
CFLAGS-libvdm_fat.a += -arch i386
CFLAGS-libCG_fat.a += -arch i386
else
CFLAGS-libvdm_fat.a += -arch x86_64 -arch ppc64
CFLAGS-libCG_fat.a += -arch x86_64 -arch ppc64
endif
endif
endif
endif
endif
endif
endif
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
CFLAGS-libvdm.a += -I/usr/local/include
CFLAGS-libCG.a += -I/usr/local/include
CFLAGS-libvdm_shared.a += -I/usr/local/include
CFLAGS-libCG_shared.a += -I/usr/local/include
endif

ifdef WINDEBUG
CFLAGS-tb_version-sl.o += -DWINDEBUG
CFLAGS-tb_version-pp.o += -DWINDEBUG
CFLAGS-tb_version-rt.o += -DWINDEBUG
endif

# Comment in -O to check for longjmp warning in vdmde.cc interface code. 
CFLAGS-vdmde.o= #-O
CFLAGS-tm.o = -DTESTSPEC
CFLAGS-be-sl.o = -DTESTSPEC
CFLAGS-be-pp.o = -DTESTSPEC
CFLAGS-be-rt.o = -DTESTSPEC
CFLAGS-test1-sl.o = -DTESTSPEC
CFLAGS-test1-pp.o = -DTESTSPEC
CFLAGS-test1-rt.o = -DTESTSPEC
CFLAGS-astaux-test-sl.o = -DTESTSPEC
CFLAGS-astaux-test-pp.o = -DTESTSPEC
CFLAGS-astaux-test-rt.o = -DTESTSPEC

LINKCHG = echo
LINKOUT = -o #Space after -o

############## Qt 

ifeq ($(strip $(QTVERSION)),5)
QTVER = -DQTVER=5
else
ifeq ($(strip $(QTVERSION)),4)
QTVER = -DQTVER=4
endif
endif

ifeq ($(strip $(OSTYPE)),Darwin)
QTFLAGS = -DQT_THREAD_SUPPORT -DQT_ACCESSIBILITY_SUPPORT -DQT_SHARED $(QTVER)
else
QTFLAGS = -DQT_THREAD_SUPPORT $(QTVER)
endif

ifdef QT_IMAGE_FILE
QTFLAGS += -DUSE_IMAGE_FILE
endif

ifeq ($(strip $(OSTYPE)),win32)
ifdef WINDEBUG
ifeq ($(strip $(QTVERSION)),4)
QTV ?= 4.8.6
QTDIR   ?= C:/Qt/$(QTV)
QTDIR_CYGWIN = $(shell cygpath -u $(QTDIR))
QTLIBFLAG = 
LIBQT    = $(QTDIR)/lib/QtGuid4.lib \
	   $(QTDIR)/lib/QtCored4.lib
QT_DLL   = $(QTDIR_CYGWIN)/bin/QtGuid4.dll \
	   $(QTDIR_CYGWIN)/bin/QtCored4.dll 
QT_CODECS = $(QTDIR_CYGWIN)/plugins/codecs/qcncodecsd4.dll \
	    $(QTDIR_CYGWIN)/plugins/codecs/qjpcodecsd4.dll \
	    $(QTDIR_CYGWIN)/plugins/codecs/qkrcodecsd4.dll \
	    $(QTDIR_CYGWIN)/plugins/codecs/qtwcodecsd4.dll
else
QTV ?= 3.3.8
QTDIR ?= C:/Qt/$(QTV)
QTDIR_CYGWIN = $(shell cygpath -u $(QTDIR))
QTLIBFLAG = 
LIBQT    = $(QTDIR)/lib/qt-mt338.lib $(QTDIR)/lib/qtmain.lib
QT_DLL   = $(QTDIR_CYGWIN)/bin/qt-mt338.dll
endif
else
ifeq ($(strip $(QTVERSION)),4)
QTV ?= 4.8.6
QTDIR ?= C:/Qt/$(QTV)
QTDIR_CYGWIN = $(shell cygpath -u $(QTDIR))
QTLIBFLAG = 
LIBQT    = $(QTDIR)/lib/QtGui4.lib \
	   $(QTDIR)/lib/QtCore4.lib
QT_DLL   = $(QTDIR_CYGWIN)/bin/QtGui4.dll \
	   $(QTDIR_CYGWIN)/bin/QtCore4.dll 
QT_CODECS = $(QTDIR_CYGWIN)/plugins/codecs/qcncodecs4.dll \
	    $(QTDIR_CYGWIN)/plugins/codecs/qjpcodecs4.dll \
	    $(QTDIR_CYGWIN)/plugins/codecs/qkrcodecs4.dll \
	    $(QTDIR_CYGWIN)/plugins/codecs/qtwcodecs4.dll
else
QTV ?= 3.3.8
QTDIR ?= C:/Qt/$(QTV)
QTDIR_CYGWIN = $(shell cygpath -u $(QTDIR))
QTLIBFLAG = 
QTLIBFLAG = 
LIBQT    = $(QTDIR)/lib/qt-mt338.lib $(QTDIR)/lib/qtmain.lib
QT_DLL   = $(QTDIR_CYGWIN)/bin/qt-mt338.dll
endif
endif
QTBINDIR = $(QTDIR)/bin
QTLIBDIR = $(QTDIR)/lib
QTINCDIR = $(QTDIR)/include
QTINCFLAG ?= -I$(QTINCDIR)
endif

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(QTVERSION)),4)
QT4DIR    ?= /usr/local/qt4
QTDIR = $(QT4DIR)

ifdef COCOA
#LIBQT    = -lm -lz -F$(QT4DIR)/lib -framework QtGui -framework QtCore -framework CoreFoundation
LIBQT    = -lm -lz -F$(QT4DIR)/lib -framework QtGui -framework QtCore
QT_LIB = 
FWQTCORE=QtCore.framework/Versions/4/QtCore
FWQTGUI=QtGui.framework/Versions/4/QtGui
OLDQTCORE=$(QT4DIR)/lib/$(FWQTCORE)
NEWQTCORE=@executable_path/../Frameworks/$(FWQTCORE)
OLDQTGUI=$(QT4DIR)/lib/$(FWQTGUI)
NEWQTGUI=@executable_path/../Frameworks/$(FWQTGUI)
else
#LIBQT    = -lQtGui -lQtCore -lm -lz -framework CoreFoundation
LIBQT    = -lQtGui -lQtCore -lm -lz
LIBQTCORE=libQtCore.4.dylib
LIBQTGUI=libQtGui.4.dylib
QT_LIB = $(QT4DIR)/lib/$(LIBQTGUI) \
	 $(QT4DIR)/lib/$(LIBQTCORE)
OLDQTCORE=$(QT4DIR)/lib/$(LIBQTCORE)
NEWQTCORE=@executable_path/../lib/$(LIBQTCORE)
OLDQTGUI=$(QT4DIR)/lib/$(LIBQTGUI)
NEWQTGUI=@executable_path/../lib/$(LIBQTGUI)
endif
LIBQCNCODECS=libqcncodecs.dylib
LIBQJPCODECS=libqjpcodecs.dylib
LIBQKRCODECS=libqkrcodecs.dylib
LIBQTWCODECS=libqtwcodecs.dylib
QT_CODECS = $(QT4DIR)/plugins/codecs/$(LIBQCNCODECS) \
	    $(QT4DIR)/plugins/codecs/$(LIBQJPCODECS) \
	    $(QT4DIR)/plugins/codecs/$(LIBQKRCODECS) \
	    $(QT4DIR)/plugins/codecs/$(LIBQTWCODECS)
QTINCDIR = $(QTDIR)/include
QTINCFLAG ?= -I$(QTINCDIR)
else
QT3DIR    ?= /usr/local/qt
QTDIR = $(QT3DIR)
#LIBQT    = -lqt-mt -lqui -lm -lz -framework CoreFoundation
LIBQT    = -lqt-mt -lqui -lm -lz
LIBQTMT=libqt-mt.3.dylib
LIBQUI=libqui.1.dylib
QT_LIB = $(QT3DIR)/lib/$(LIBQTMT) \
	 $(QT3DIR)/lib/$(LIBQUI)
#OLDQTMT=$(QT3DIR)/lib/$(LIBQTMT)
OLDQTMT=$(LIBQTMT)
NEWQTMT=@executable_path/../lib/$(LIBQTMT)
#OLDQUI=$(QT3DIR)/lib/$(LIBQUI)
OLDQUI=$(LIBQUI)
NEWQUI=@executable_path/../lib/$(LIBQUI)
QTINCDIR = $(QTDIR)/include
QTINCFLAG ?= -I$(QTINCDIR)
endif
QTLIBFLAG = -L$(QTDIR)/lib
QTBINDIR = $(QTDIR)/bin
QTLIBDIR = $(QTDIR)/lib
endif

ifeq ($(strip $(OSTYPE)),Linux)
ifeq ($(strip $(QTVERSION)),5)
QT5DIR    ?= /usr/lib/qt5
QTLIBFLAG = -L$(QTLIBDIR)
LIBQT    = -lQt5Gui -lQt5Core -lQt5Widgets
QTBINDIR ?= $(QT5DIR)/bin
QTLIBDIR ?= /usr/lib
QTINCDIR ?= /usr/include
QTINCFLAG ?= -I$(QTINCDIR) -fPIC
else
ifeq ($(strip $(QTVERSION)),4)
QT4DIR    ?= /usr/lib/qt4
QTLIBFLAG = -L$(QTLIBDIR)
LIBQT    = -lQtGui -lQtCore
QTBINDIR ?= $(QT4DIR)/bin
QTLIBDIR ?= /usr/lib
QTINCDIR ?= /usr/include
QTINCFLAG ?= -I$(QTINCDIR)
else
QTDIR    ?= /usr/lib/qt-3.3
QT3DIR   ?= $(QTDIR)
QTLIBFLAG = -L$(QT3DIR)/lib
LIBQT    = -lqt-mt
QTBINDIR ?= $(QT3DIR)/bin
QTLIBDIR ?= $(QT3DIR)/lib
QTINCDIR ?= $(QT3DIR)/include
QTINCFLAG ?= -I$(QTINCDIR)
endif
endif
endif

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
ifeq ($(strip $(QTVERSION)),4)
QT4DIR    ?= /usr/lib/qt4
QTLIBFLAG = -L$(QTLIBDIR)
#LIBQT    = -lQtGui -lQtCore -lm
LIBQT    = -lQtGui -lQtCore
QTBINDIR ?= $(QT4DIR)/bin
QTLIBDIR ?= /usr/lib
QTINCDIR ?= /usr/include
QTINCFLAG ?= -I$(QTINCDIR)
else
QTDIR    ?= /usr/lib/qt-3.3
QT3DIR   ?= $(QTDIR)
QTLIBFLAG = -L$(QT3DIR)/lib
LIBQT    = -lqt-mt
QTBINDIR ?= $(QT3DIR)/bin
QTLIBDIR ?= $(QT3DIR)/lib
QTINCDIR ?= $(QT3DIR)/include
QTINCFLAG ?= -I$(QTINCDIR)
endif
endif

ifeq ($(strip $(OSTYPE)),SunOS)
ifeq ($(strip $(QTVERSION)),4)
QT4DIR    ?= /usr/local/qt4
QTDIR    ?= $(QT4DIR)
LIBQT    = -lQtGui -lQtCore -lm
QT_LIB = $(QT4DIR)/lib/libQtGui.so.4 \
	 $(QT4DIR)/lib/libQtCore.so.4
LIBQCNCODECS=libqcncodecs.so
LIBQJPCODECS=libqjpcodecs.so
LIBQKRCODECS=libqkrcodecs.so
LIBQTWCODECS=libqtwcodecs.so
QT_CODECS = $(QT4DIR)/plugins/codecs/$(LIBQCNCODECS) \
	    $(QT4DIR)/plugins/codecs/$(LIBQJPCODECS) \
	    $(QT4DIR)/plugins/codecs/$(LIBQKRCODECS) \
	    $(QT4DIR)/plugins/codecs/$(LIBQTWCODECS)
else
QT3DIR    ?= /usr/local/qt
QTDIR    ?= $(QT3DIR)
LIBQT    = -lqt-mt -lSM -lICE -lpthread -lXext -lX11 -lm  # -lXft
endif
QTLIBFLAG = -L$(QTDIR)/lib -L/usr/X11R6/lib
QTBINDIR = $(QTDIR)/bin
QTLIBDIR = $(QTDIR)/lib
QTINCDIR = $(QTDIR)/include
QTINCFLAG ?= -I$(QTINCDIR)
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
ifeq ($(strip $(QTVERSION)),4)
QT4DIR    ?= /usr/local/qt4
QTDIR    ?= $(QT4DIR)
LIBQT    = -lQtGui -lQtCore -lm
QT_LIB = $(QT4DIR)/lib/libQtGui.so.4 \
	 $(QT4DIR)/lib/libQtCore.so.4
else
QT3DIR    ?= /usr/local/qt
QTDIR    ?= $(QT3DIR)
LIBQT    = -lqt-mt -lSM -lICE -lpthread -lXext -lX11 -lm  # -lXft
endif
QTLIBDIR ?= $(QTDIR)/lib
QTLIBFLAG = -L$(QTLIBDIR) -L/usr/X11R6/lib
QTBINDIR ?= $(QTDIR)/bin
QTINCDIR ?= $(QTDIR)/include
QTINCFLAG ?= -I$(QTINCDIR)
endif

QTMOC     ?= $(QTBINDIR)/moc
QTLRELEASE ?= $(QTBINDIR)/lrelease

SRCMOC = moc_mainF.cc \
	 moc_optionsF.cc \
	 moc_codeF.cc \
	 moc_errorF.cc \
	 moc_interpreterF.cc \
	 moc_browserF.cc \
	 moc_logF.cc \
	 moc_interfacesF.cc \
	 moc_toolOptionsDialog.cc \
	 moc_pogF.cc \
	 moc_searchF.cc \
	 moc_roseF.cc

SRCLRELEASE = vdm_ja_JP.ts

QT_IMG = img
QMFILES = vdm_ja_JP.qm

vdm_%.qm: vdm_%.ts
ifeq ($(strip $(OSTYPE)),Linux)
	$(QTLRELEASE) $^
else
	$(QTLRELEASE) $^ -qm $@
endif

CFLAGS-libqtgui-pp.a += $(QTINCFLAG) $(QTFLAGS)
CFLAGS-libqtgui-sl.a += $(QTINCFLAG) $(QTFLAGS)
CFLAGS-libqtgui-rt.a += $(QTINCFLAG) $(QTFLAGS)

ifeq ($(strip $(OSTYPE)),win32)
CFLAGS-libqtguiROSE-pp.a += $(QTINCFLAG) $(QTFLAGS) -D_USE_ROSE
CFLAGS-libqtguiROSE-rt.a += $(QTINCFLAG) $(QTFLAGS) -D_USE_ROSE
endif

CFLAGS-vdm-pp.o += $(QTINCFLAG) $(QTFLAGS)
CFLAGS-vdm-sl.o += $(QTINCFLAG) $(QTFLAGS)
CFLAGS-vdm-rt.o += $(QTINCFLAG) $(QTFLAGS)

ifeq ($(strip $(OSTYPE)),win32)
moc_%.cc: %.h
	$(QTMOC) $(QTVER) -i -f$(basename $<).h -o $@ $^
else
moc_%.cc: %.h
	$(QTMOC) $(QTVER) -o $@ $^
endif

OBJMOC = $(patsubst %.cc, %.o, $(SRCMOC))

LIB_QTGUI_PP = libqtgui-pp.a
LIB_QTGUI_SL = libqtgui-sl.a
LIB_QTGUI_RT = libqtgui-rt.a

ifeq ($(strip $(OSTYPE)),win32)
LIB_QTGUIROSE_PP = libqtguiROSE-pp.a
LIB_QTGUIROSE_RT = libqtguiROSE-rt.a
endif

LIB_QTGUI_O = $(OBJMOC) \
	      mainF.o \
	      optionsF.o \
	      codeF.o \
	      errorF.o \
	      interpreterF.o \
	      browserF.o \
	      logF.o \
	      interfacesF.o \
	      interface2TB.o \
	      toolOptionsDialog.o \
	      toolboxthread.o \
	      splashF.o \
	      customevents.o \
	      pogF.o \
	      searchF.o \
	      localeutil.o \
	      qtport.o \
	      imagedata.o

LIB_QTGUI_SL_O = $(LIB_QTGUI_O:%.o=%.o)
LIB_QTGUI_PP_O = $(LIB_QTGUI_O:%.o=%.o) roseF.o
LIB_QTGUI_RT_O = $(LIB_QTGUI_O:%.o=%.o) roseF.o

$(LIB_QTGUI_PP): $(LIB_QTGUI_PP)($(LIB_QTGUI_PP_O))
$(LIB_QTGUI_SL): $(LIB_QTGUI_SL)($(LIB_QTGUI_SL_O))
$(LIB_QTGUI_RT): $(LIB_QTGUI_RT)($(LIB_QTGUI_RT_O))

ifeq ($(strip $(OSTYPE)),win32)
$(LIB_QTGUIROSE_PP): $(LIB_QTGUIROSE_PP)($(LIB_QTGUI_PP_O))
$(LIB_QTGUIROSE_RT): $(LIB_QTGUIROSE_RT)($(LIB_QTGUI_RT_O))
endif
############## end of Qt

################################## Solaris ##############################
ifeq ($(strip $(OSTYPE)),SunOS)

CCPATH = /usr/sfw/bin/
# !!  Do not change DEBUG and OPTIMIZE. Instead create an Override.mak file 
# !!  containing the wanted settings 
OSVERSION = $(shell uname -r)
OSV_MAJOR = $(word 1, $(subst ., ,$(strip $(OSVERSION))))
OSV_MINOR = $(word 2, $(subst ., ,$(strip $(OSVERSION))))
GCCVERSION = $(shell $(CXX) -dumpversion)
GCC_MAJOR = $(word 1, $(subst ., ,$(strip $(GCCVERSION))))
GCC_MINOR = $(word 2, $(subst ., ,$(strip $(GCCVERSION))))
#DEBUG = -g

ifdef NOOPT
OPTIMIZE = -mtune=pentiumpro
else
OPTIMIZE = -mtune=pentiumpro -O2
endif

AR=ar 
ARCH_DEFINES = -D__SunOS__
LINK_DEBUG= 

LNKOPT += -L/usr/local/lib -lposix4 -lsocket

FLEXROOT?=/usr/sfw
CPUTYPE = $(shell uname -p)
DISTARCH = solaris10-$(CPUTYPE)
DISTCC = gcc-$(GCC_MAJOR).$(GCC_MINOR)
STRIP = /usr/sfw/bin/gstrip -g

JDKHOME ?= /usr/java
ANTHOME ?= /usr/local/ant
JAVACCHOME ?= /usr/local/javacc-5.0
JAVACC ?= $(JAVACCHOME)/bin/javacc
endif
# End of Solaris 2.10

################################## FreeBSD ##############################
ifeq ($(strip $(OSTYPE)),FreeBSD)

CCPATH = /usr/bin/
# !!  Do not change DEBUG and OPTIMIZE. Instead create an Override.mak file 
# !!  containing the wanted settings 
OSVERSION = $(shell uname -r)
OSV_MAJOR = $(word 1, $(subst ., ,$(strip $(OSVERSION))))
OSV_MINOR = $(word 2, $(subst ., ,$(strip $(OSVERSION))))
GCCVERSION = $(shell $(CXX) -dumpversion)
GCC_MAJOR = $(word 1, $(subst ., ,$(strip $(GCCVERSION))))
GCC_MINOR = $(word 2, $(subst ., ,$(strip $(GCCVERSION))))
#DEBUG = -g

CPUTYPE = $(shell uname -m)

OPTIMIZE += -O3

AR=ar 
MAKE=gmake
#ARCH_DEFINES = -D__FreeBSD__ #defined by compiler
ARCH_DEFINES =
LINK_DEBUG= 

LNKOPT += -L/usr/local/lib -liconv

READLINE_LIB += -lreadline

FLEXROOT?=/usr
CPUTYPE = $(shell uname -p)
DISTARCH    = FreeBSD-$(OSV_MAJOR)-$(CPUTYPE)
DISTCC = gcc-$(GCC_MAJOR).$(GCC_MINOR)
STRIP = /usr/bin/strip -g

JDKHOME ?= /usr/local
ANTHOME ?= /usr/local
JAVACCHOME ?= /usr/local/javacc-5.0
JAVACC ?= $(JAVACCHOME)/bin/javacc
endif
# End of FreeBSD

######################################## Linux ##############################
ifeq ($(strip $(OSTYPE)),Linux)
# !!  Do not change DEBUG and OPTIMIZE. Instead create an Override.mak file 
# !!  containing the wanted settings 
OSVERSION = $(shell uname -r)
OSV_MAJOR = $(word 1, $(subst ., ,$(strip $(OSVERSION))))
OSV_MINOR = $(word 2, $(subst ., ,$(strip $(OSVERSION))))
GCCVERSION = $(shell $(CXX) -dumpversion)
GCC_MAJOR = $(word 1, $(subst ., ,$(strip $(GCCVERSION))))
GCC_MINOR = $(word 2, $(subst ., ,$(strip $(GCCVERSION))))

#DEBUG= -g

CCPATH = /usr/bin/
ARCH_DEFINES = -D__linux__ $(NOCG)
LINK_DEBUG=

ifeq ($(strip $(CXX)),$(CCPATH)clang++)
ARCH += -std=c++11
WARNINGS += -Wno-deprecated-register
endif

TUNEOPT=-mtune
ifeq ($(strip $(GCC_MAJOR)),3)
ifeq ($(strip $(GCC_MINOR)),2)
TUNEOPT=-mcpu
else
ifeq ($(strip $(GCC_MINOR)),3)
TUNEOPT=-mcpu
endif
endif
endif

STATIC = #-static 
FLEXROOT?=/usr
CPUTYPE = $(shell uname -m)

ifndef NOOPT
OPTIMIZE +=-O3
endif

DISTARCH = Linux-$(CPUTYPE)
DISTCC = gcc-$(GCC_MAJOR).$(GCC_MINOR)

STRIP = strip -g #otherwise all symbols are removed.
CXXLIB ?= $(shell ldconfig -p | grep libstdc++ | awk -F ' ' '{print $$4}')

READLINE_LIB += -lreadline

JDKHOME ?= /usr
ANTHOME ?= /usr
JAVACCHOME ?= /usr/local/javacc-5.0
JAVACC ?= $(JAVACCHOME)/bin/javacc

endif 
# End of Linux

#################################### CYGWIN_NT ##########################
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
# !!  Do not change DEBUG and OPTIMIZE. Instead create an Override.mak file 
# !!  containing the wanted settings 
OSVERSION = $(shell uname -r)
OSV_MAJOR = $(word 1, $(subst ., ,$(strip $(OSVERSION))))
OSV_MINOR = $(word 2, $(subst ., ,$(strip $(OSVERSION))))
GCCVERSION = $(shell $(CXX) -dumpversion)
GCC_MAJOR = $(word 1, $(subst ., ,$(strip $(GCCVERSION))))
GCC_MINOR = $(word 2, $(subst ., ,$(strip $(GCCVERSION))))

#DEBUG= -g
VPATH=.
BISON2=1
YACC=bison

CCPATH = /usr/bin/
ARCH_DEFINES = -D__Cygwin__ -D__GNU_LIBRARY__ $(NOCG)
LINK_DEBUG=

ARCH += -std=gnu++11

LNKOPT += -liconv

READLINE_LIB += -lreadline

TUNEOPT=-mtune
ifeq ($(strip $(GCC_MAJOR)),3)
ifeq ($(strip $(GCC_MINOR)),2)
TUNEOPT=-mcpu
else
ifeq ($(strip $(GCC_MINOR)),3)
TUNEOPT=-mcpu
endif
endif
endif

STATIC = #-static 
FLEXROOT?=/usr
CPUTYPE = $(shell uname -m)

ifndef NOOPT
OPTIMIZE +=-O3
endif

DISTARCH = Cygwin-$(CPUTYPE)
DISTCC = gcc-$(GCC_MAJOR).$(GCC_MINOR)

STRIP = strip -g #otherwise all symbols are removed.

JDKHOME ?= /usr
ANTHOME ?= /usr
JAVACCHOME ?= /usr/local/javacc-5.0
JAVACC ?= $(JAVACCHOME)/bin/javacc

endif 
# End of CYGWIN_NT

######################################### win32 ##############################
ifeq ($(strip $(OSTYPE)),win32)

ifneq ("$(wildcard /usr/bin/link.exe)","")
$(error "Error: /usr/bin/link.exe should be renamed.")
endif

#UNAME = $(shell uname)
#REALOS = $(word 1, $(subst -, ,$(strip $(UNAME))))

CLSTRING = $(shell cl >& msc_tmp;cat msc_tmp | head -1)
IS32 = $(word 2, $(CLSTRING)) 
ifeq ($(strip $(IS32)),32-bit)
CLVERSION = $(word 7, $(CLSTRING)) 
CLCPUTYPE = $(word 9, $(CLSTRING))
else
CLVERSION = $(word 6, $(CLSTRING)) 

# "80x86" or "x64"
CLCPUTYPE = $(word 8, $(CLSTRING))
ifeq ($(strip $(CLCPUTYPE)),for) # for Express 
CLVERSION = $(word 7, $(CLSTRING)) 
CLCPUTYPE = $(word 9, $(CLSTRING))
endif
endif

# installer: "NSIS" or "WISE"
INSTALLER ?= NSIS

# VC++ Version
#MSVC++ 4.x  _MSC_VER == 1000
#MSVC++ 5.0  _MSC_VER == 1100
#MSVC++ 6.0  _MSC_VER == 1200
#MSVC++ 7.0  _MSC_VER == 1300
#MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
#MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
#MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
#MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
#MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
#MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
#MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
#MSVC++ 14.1 _MSC_VER == 1910 (Visual Studio 2017)

# 14 : VC8
# 15 : VC9
# 16 : VC10
# 17 : VC11
CL_MAJOR = $(word 1, $(subst ., ,$(strip $(CLVERSION))))

# !!  Do not change DEBUG and OPTIMIZE. Instead create an Override.mak file 
# !!  containing the wanted settings 
#DEBUG=/Zi
DEBUG=

ifndef MSCVER
MSCVERSTR = $(shell cl >& msc_tmp;cat msc_tmp | head -1)
VERSTR1 = $(word 1, $(subst ., ,$(strip $(word 8, $(MSCVERSTR)))))
VERSTR2 = $(word 1, $(subst ., ,$(strip $(word 7, $(MSCVERSTR)))))
ifeq ($(strip $(VERSTR1)),12)
$(error "Error: Visual Studio 6 has been no longer supported.")
else
ifeq ($(strip $(VERSTR2)),14)
MSCVER = VC8 # VC++ 8
else
ifeq ($(strip $(VERSTR2)),15)
MSCVER = VC9 # VC++ 9
else
ifeq ($(strip $(VERSTR2)),16)
MSCVER = VC10 # VC++ 10
else
ifeq ($(strip $(VERSTR2)),17)
MSCVER = VC11 # VC++ 11
else
MSCVER = VC11 # VC++ 11
endif #17
endif #16
endif #15
endif #14
endif #12
endif #MSCVER

ifeq ($(strip $(MSCVER)),VC8)
VSVER=vs8
else
ifeq ($(strip $(MSCVER)),VC9)
VSVER=vs9
else
ifeq ($(strip $(MSCVER)),VC10)
VSVER=vs10
else
ifeq ($(strip $(MSCVER)),VC11)
VSVER=vs11
else
$(error "Error: Visual Studio 6 has been no longer supported.")
endif #VC11
endif #VC10
endif #VC9
endif #VC8

# Use -Ox but disable buggy -Oy and -Og optimization.
OPTIMIZE=/O2

WCHAR= -D_UNICODE
LINK_DEBUG= 

# manifest tool
MT=mt.exe

ifeq ($(strip $(REALOS)),CYGWIN_NT)
#LINKCHG = $(VPATH)/linkchg.pl 1
LINKCHG = /usr/bin/perl $(VPATH)/linkchg.pl 1
# FIXPARSER = $(VPATH)/fixparser.pl
else
LINKCHG = ./linkchg.pl 1
# FIXPARSER = ./fixparser.pl
endif

LINKOUT = /Fe
ifdef WIN32_PROFILE
AR=LIB
else
#AR=ar
AR=/usr/bin/ar
endif
CCPATH = 

#CCC = cl /nologo /Fmvppuml.map /D_CRT_SECURE_NO_DEPRECATE
ifeq ($(strip $(MSCVER)),VC9)
CCC = cl /nologo /Fmvppuml.map -D_BIND_TO_CURRENT_VCLIBS_VERSION=1
CC = cl /nologo /Fmvppuml.map -D_BIND_TO_CURRENT_VCLIBS_VERSION=1
else
CCC = cl /nologo /Fmvppuml.map
CC = cl /nologo /Fmvppuml.map 
endif

SHADOWINC = $(patsubst %,-I%,$(subst :, ,$(VPATH)))

WARNINGS = #-W3
WINGUIOBJS = # winprint.o
%: %.c
#  commands to execute (built-in):
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

OUTPUT_OPTION = /Fo$@ # No space after /Fo
LIB_OUTPUT_OPTION = /Fo$% # No space after /Fo
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) /c
# _NOTHREADS is defined to avoid including windows.h in stl_config.h

ARCH_DEFINES = $(MD) /EHsc $(SRCTYPE) /GR /D__WIN32__ 

SRCTYPE = $(TYPE$(suffix $<))
SRCNAME = $(type$(suffix $<))
TYPE.cc = /TP
TYPE.c = /TC
type.cc = /Tp
type.c = /Tc
# -ldl not needed on Irix.

ifdef WINDEBUG
ifeq ($(strip $(MSCVER)),VC8)
VDMMANIFEST = vdmtools_vc8d.manifest
CLIMANIFEST = cli_vc8d.manifest
GUIMANIFEST = gui_vc8d.manifest
ROSEMANIFEST = rose_vc8d.manifest
else
ifeq ($(strip $(MSCVER)),VC9)
VDMMANIFEST = vdmtools_vc9d.manifest
CLIMANIFEST = cli_vc9d.manifest
GUIMANIFEST = gui_vc9d.manifest
ROSEMANIFEST = rose_vc9d.manifest
else
ifeq ($(strip $(MSCVER)),VC10)
#VDMMANIFEST = vdmtools_vc10d.manifest
#CLIMANIFEST = cli_vc10d.manifest
#GUIMANIFEST = gui_vc10d.manifest
#ROSEMANIFEST = rose_vc10d.manifest
VDMMANIFEST = vdmtools_vc10.manifest
CLIMANIFEST = cli_vc10.manifest
GUIMANIFEST = gui_vc10.manifest
ROSEMANIFEST = rose_vc10.manifest
else
ifeq ($(strip $(MSCVER)),VC11)
VDMMANIFEST = vdmtools_vc11.manifest
CLIMANIFEST = cli_vc11.manifest
GUIMANIFEST = gui_vc11.manifest
ROSEMANIFEST = rose_vc11.manifest
endif #VC11
endif #VC10
endif #VC9
endif #VC8
else
ifeq ($(strip $(MSCVER)),VC8)
VDMMANIFEST = vdmtools_vc8.manifest
CLIMANIFEST = cli_vc8.manifest
GUIMANIFEST = gui_vc8.manifest
ROSEMANIFEST = rose_vc8.manifest
else
ifeq ($(strip $(MSCVER)),VC9)
VDMMANIFEST = vdmtools_vc9.manifest
CLIMANIFEST = cli_vc9.manifest
GUIMANIFEST = gui_vc9.manifest
ROSEMANIFEST = rose_vc9.manifest
else
ifeq ($(strip $(MSCVER)),VC10)
VDMMANIFEST = vdmtools_vc10.manifest
CLIMANIFEST = cli_vc10.manifest
GUIMANIFEST = gui_vc10.manifest
ROSEMANIFEST = rose_vc10.manifest
else
ifeq ($(strip $(MSCVER)),VC11)
VDMMANIFEST = vdmtools_vc11.manifest
CLIMANIFEST = cli_vc11.manifest
GUIMANIFEST = gui_vc11.manifest
ROSEMANIFEST = rose_vc11.manifest
endif #VC11
endif #VC10
endif #VC9
endif #VC8
endif #WINDEBUG

WINRESGDE-SL = vdmgde-sl.res
WINRESGDE-PP = vdmgde-pp.res
WINRESGDE-RT = vdmgde-rt.res

WINRES-SL = vdmde-sl.res
WINRES-PP = vdmde-pp.res
WINRES-RT = vdmde-rt.res

RC = rc
EXE = .exe
GETOPT_O = getopt.o

ifdef STATIC
ifdef WINDEBUG
MD=/MTd
MDLIBS=msvcrtd.lib msvcprtd.lib
else
MD=/MT
MDLIBS=msvcrt.lib msvcprt.lib
endif
else
ifdef WINDEBUG
MD=/MDd
MDLIBS=msvcrtd.lib msvcprtd.lib
else
MD=/MD
MDLIBS=msvcrt.lib msvcprt.lib
endif
endif

WINCOMMONLIBS = $(MDLIBS) kernel32.lib user32.lib advapi32.lib delayimp.lib oldnames.lib

ifdef WINDEBUG
ifeq ($(strip $(MSCVER)),VC8)
WINOLELIBS = mfc80ud.lib mfcs80ud.lib atlsd.lib ole32.lib oleaut32.lib uuid.lib
else
ifeq ($(strip $(MSCVER)),VC9)
WINOLELIBS = mfc90ud.lib mfcs90ud.lib atlsd.lib ole32.lib oleaut32.lib uuid.lib
else
ifeq ($(strip $(MSCVER)),VC10)
WINOLELIBS = mfc100ud.lib mfcs100ud.lib atlsd.lib ole32.lib oleaut32.lib uuid.lib
else
ifeq ($(strip $(MSCVER)),VC11)
WINOLELIBS = mfc110ud.lib mfcs110ud.lib atlsd.lib ole32.lib oleaut32.lib uuid.lib
endif #VC11
endif #VC10
endif #VC9
endif #VC8
else
ifeq ($(strip $(MSCVER)),VC8)
WINOLELIBS = mfc80u.lib mfcs80u.lib atls.lib ole32.lib oleaut32.lib uuid.lib
else
ifeq ($(strip $(MSCVER)),VC9)
WINOLELIBS = mfc90u.lib mfcs90u.lib atls.lib ole32.lib oleaut32.lib uuid.lib
else
ifeq ($(strip $(MSCVER)),VC10)
WINOLELIBS = mfc100u.lib mfcs100u.lib atls.lib ole32.lib oleaut32.lib uuid.lib
else
ifeq ($(strip $(MSCVER)),VC11)
WINOLELIBS = mfc110u.lib mfcs110u.lib atls.lib ole32.lib oleaut32.lib uuid.lib
endif #VC11
endif #VC10
endif #VC9
endif #VC8
endif #WINDEBUG

WINLIBS = $(WINCOMMONLIBS)
ROSELIBS = $(WINOLELIBS) $(WINCOMMONLIBS)
MANIFEST = /MANIFEST

# STACK: With zero stack we get unlimited stack. But lets try if this is
# enough. 

STACK = /STACK:0x400000

ifeq ($(strip $(QTVERSION)),3)
ENTRY_GUI = /ENTRY:WinMainCRTStartup
ENTRY_CLI = /ENTRY:mainCRTStartup
else #Qt4/5
ENTRY_GUI = /ENTRY:mainCRTStartup
ENTRY_CLI = /ENTRY:mainCRTStartup
endif

ifeq ($(strip $(CLCPUTYPE)),80x86)
MACHINE=/MACHINE:IX86
SUBSYSTEM_GUI = /SUBSYSTEM:WINDOWS,5.0
SUBSYSTEM_CLI = /SUBSYSTEM:CONSOLE,5.0
else
ifeq ($(strip $(CLCPUTYPE)),x64)
MACHINE=/MACHINE:X64
SUBSYSTEM_GUI = /SUBSYSTEM:WINDOWS,6.1
SUBSYSTEM_CLI = /SUBSYSTEM:CONSOLE,6.1
else
endif
endif

cputype:
	echo "$(CLSTRING)"
	echo "$(CLVERSION)"
	echo "$(CL_MAJOR)"
	echo "$(CLCPUTYPE)"

WINLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	$(WINLIBS) $(STACK)

WINCONSOLELINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_CLI) $(ENTRY_CLI) \
	$(WINLIBS) $(STACK)

ifdef WIN32_PROFILE
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) \
	/PROFILE /FIXED:NO \
	$(ROSELIBS) $(STACK)
else
ifdef WINDEBUG
ifeq ($(strip $(MSCVER)),VC8)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc80ud.dll \
        $(ROSELIBS) $(STACK)
else
ifeq ($(strip $(MSCVER)),VC9)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc90ud.dll \
        $(ROSELIBS) $(STACK)
else
ifeq ($(strip $(MSCVER)),VC10)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc100ud.dll \
        $(ROSELIBS) $(STACK)
else
ifeq ($(strip $(MSCVER)),VC11)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc110ud.dll \
        $(ROSELIBS) $(STACK)
endif #VC11
endif #VC10
endif #VC9
endif #VC8
else
ifeq ($(strip $(MSCVER)),VC8)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc80u.dll \
        $(ROSELIBS) $(STACK)
else
ifeq ($(strip $(MSCVER)),VC9)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc90u.dll \
        $(ROSELIBS) $(STACK)
else
ifeq ($(strip $(MSCVER)),VC10)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc100u.dll \
        $(ROSELIBS) $(STACK)
else
ifeq ($(strip $(MSCVER)),VC11)
AFXLINK = /link /FORCE:MULTIPLE /RELEASE /NODEFAULTLIB \
	/INCREMENTAL:NO /NOLOGO $(MACHINE) $(MANIFEST)\
	$(SUBSYSTEM_GUI) $(ENTRY_GUI) \
	/DELAYLOAD:ole32.dll /DELAYLOAD:oleaut32.dll \
	/DELAYLOAD:advapi32.dll /DELAYLOAD:mfc110u.dll \
        $(ROSELIBS) $(STACK)
endif #VC11
endif #VC10
endif #VC9
endif #VC8
endif #WINDEBUG
endif #WIN32_PROFILE

ifeq ($(strip $(MSCVER)),VC8)
REDISTDIR=/cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 8/VC/redist/x86
MSVCCRTDIR=$(REDISTDIR)/Microsoft.VC80.CRT
MSVCMFCDIR=$(REDISTDIR)/Microsoft.VC80.MFC
#else
endif

ifeq ($(strip $(MSCVER)),VC9)
ifeq ($(strip $(CLCPUTYPE)),x64)
REDISTDIR=/cygdrive/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 9.0/VC/redist/amd64
else
REDISTDIR=/cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 9.0/VC/redist/x86
endif
MSVCCRTDIR=$(REDISTDIR)/Microsoft.VC90.CRT
MSVCMFCDIR=$(REDISTDIR)/Microsoft.VC90.MFC
#else
endif

ifeq ($(strip $(MSCVER)),VC10)
ifeq ($(strip $(CLCPUTYPE)),x64)
REDISTDIR=/cygdrive/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 10.0/VC/redist/amd64
else
REDISTDIR=/cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 10.0/VC/redist/x86
endif
MSVCCRTDIR=$(REDISTDIR)/Microsoft.VC100.CRT
MSVCMFCDIR=$(REDISTDIR)/Microsoft.VC100.MFC
#else
endif

ifeq ($(strip $(MSCVER)),VC11)
ifeq ($(strip $(CLCPUTYPE)),x64)
REDISTDIR=/cygdrive/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 11.0/VC/redist/amd64
else
REDISTDIR=/cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 11.0/VC/redist/x86
endif
MSVCCRTDIR=$(REDISTDIR)/Microsoft.VC110.CRT
MSVCMFCDIR=$(REDISTDIR)/Microsoft.VC110.MFC
endif #VC11
#endif #VC10
#endif #VC9
#endif #VC8

redist:
	ls $(MSVSCRTDIR)
	ls $(MSVSMFCDIR)
 
LIB_DL = 
LIBMATH= 
WIN_DIST = $(VPATH)/win32
WIN_DIST_BIN = $(WIN_DIST)/bin
WIN_DIST_CORBA = $(WIN_DIST)/corba
FLEXROOT?=/usr
DISTARCH = i386-win32
TARFOLLOWLINKS=

ifdef WINDEBUG
WIN_DIST_CRT_BIN=$(WIN_DIST_BIN)
WIN_DIST_MFC_BIN=$(WIN_DIST_BIN)
else
ifeq ($(strip $(MSCVER)),VC8)
WIN_DIST_CRT_BIN=$(WIN_DIST_BIN)
WIN_DIST_MFC_BIN=$(WIN_DIST_BIN)
#WIN_DIST_CRT_BIN=$(MSVCCRTDIR)
#WIN_DIST_MFC_BIN=$(MSVCMFCDIR)
else
ifeq ($(strip $(CLCPUTYPE)),x64)
WIN_DIST_CRT_BIN=$(MSVCCRTDIR)
WIN_DIST_MFC_BIN=$(MSVCMFCDIR)
else
WIN_DIST_CRT_BIN=$(WIN_DIST_BIN)
WIN_DIST_MFC_BIN=$(WIN_DIST_BIN)
#WIN_DIST_CRT_BIN=$(MSVCCRTDIR)
#WIN_DIST_MFC_BIN=$(MSVCMFCDIR)
endif
endif
endif

ifdef WINDEBUG
ifeq ($(strip $(MSCVER)),VC8)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/Microsoft.VC80.DebugCRT.manifest \
	$(WIN_DIST_CRT_BIN)/msvcm80d.dll \
	$(WIN_DIST_CRT_BIN)/msvcp80d.dll \
	$(WIN_DIST_CRT_BIN)/msvcr80d.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/Microsoft.VC80.DebugMFC.manifest \
	$(WIN_DIST_MFC_BIN)/mfc80d.dll \
	$(WIN_DIST_MFC_BIN)/mfc80ud.dll \
	$(WIN_DIST_MFC_BIN)/mfcm80d.dll \
	$(WIN_DIST_MFC_BIN)/mfcm80ud.dll
else
ifeq ($(strip $(MSCVER)),VC9)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/Microsoft.VC90.DebugCRT.manifest \
	$(WIN_DIST_CRT_BIN)/msvcm90d.dll \
	$(WIN_DIST_CRT_BIN)/msvcp90d.dll \
	$(WIN_DIST_CRT_BIN)/msvcr90d.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/Microsoft.VC90.DebugMFC.manifest \
	$(WIN_DIST_MFC_BIN)/mfc90d.dll \
	$(WIN_DIST_MFC_BIN)/mfc90ud.dll \
	$(WIN_DIST_MFC_BIN)/mfcm90d.dll \
	$(WIN_DIST_MFC_BIN)/mfcm90ud.dll
else
ifeq ($(strip $(MSCVER)),VC10)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/msvcp100d.dll \
	$(WIN_DIST_CRT_BIN)/msvcr100d.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/mfc100d.dll \
	$(WIN_DIST_MFC_BIN)/mfc100ud.dll \
	$(WIN_DIST_MFC_BIN)/mfcm100d.dll \
	$(WIN_DIST_MFC_BIN)/mfcm100ud.dll
else
ifeq ($(strip $(MSCVER)),VC11)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/msvcp110d.dll \
	$(WIN_DIST_CRT_BIN)/msvcr110d.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/mfc110d.dll \
	$(WIN_DIST_MFC_BIN)/mfc110ud.dll \
	$(WIN_DIST_MFC_BIN)/mfcm110d.dll \
	$(WIN_DIST_MFC_BIN)/mfcm110ud.dll
endif #VC11
endif #VC10
endif #VC9
endif #VC8
else
ifeq ($(strip $(MSCVER)),VC8)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/Microsoft.VC80.CRT.manifest \
	$(WIN_DIST_CRT_BIN)/msvcm80.dll \
	$(WIN_DIST_CRT_BIN)/msvcp80.dll \
	$(WIN_DIST_CRT_BIN)/msvcr80.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/Microsoft.VC80.MFC.manifest \
	$(WIN_DIST_MFC_BIN)/mfc80.dll \
	$(WIN_DIST_MFC_BIN)/mfc80u.dll \
	$(WIN_DIST_MFC_BIN)/mfcm80.dll \
	$(WIN_DIST_MFC_BIN)/mfcm80u.dll
else
ifeq ($(strip $(MSCVER)),VC9)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/Microsoft.VC90.CRT.manifest \
	$(WIN_DIST_CRT_BIN)/msvcm90.dll \
	$(WIN_DIST_CRT_BIN)/msvcp90.dll \
	$(WIN_DIST_CRT_BIN)/msvcr90.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/Microsoft.VC90.MFC.manifest \
	$(WIN_DIST_MFC_BIN)/mfc90.dll \
	$(WIN_DIST_MFC_BIN)/mfc90u.dll \
	$(WIN_DIST_MFC_BIN)/mfcm90.dll \
	$(WIN_DIST_MFC_BIN)/mfcm90u.dll
else
ifeq ($(strip $(MSCVER)),VC10)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/msvcp100.dll \
	$(WIN_DIST_CRT_BIN)/msvcr100.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/mfc100.dll \
	$(WIN_DIST_MFC_BIN)/mfc100u.dll \
	$(WIN_DIST_MFC_BIN)/mfcm100.dll \
	$(WIN_DIST_MFC_BIN)/mfcm100u.dll
else
ifeq ($(strip $(MSCVER)),VC11)
WIN_CRT_DLL = $(WIN_DIST_CRT_BIN)/msvcp110.dll \
	$(WIN_DIST_CRT_BIN)/msvcr110.dll
WIN_MFC_DLL = $(WIN_DIST_MFC_BIN)/mfc110.dll \
	$(WIN_DIST_MFC_BIN)/mfc110u.dll \
	$(WIN_DIST_MFC_BIN)/mfcm110.dll \
	$(WIN_DIST_MFC_BIN)/mfcm110u.dll
endif #VC11
endif #VC10
endif #VC9
endif #VC8
endif #WINDEBUG

# QUANTIFY = quantify -g++=yes $(QUANTIFY_COLLECTOR) \
#      -collection-granularity=function -record-register-window-traps=yes \
#      -record-system-calls=yes 
# QUANTIFY_COLLECTOR = -collector=/usr/local/package/gcc-2.7.2/lib/gcc-lib/sparc-sun-sunos4.1.3/2.7.2/ld

JDKHOME ?= C:/j2sdk1.4.2_11
NTHOME ?= /usr/local/ant
JAVACCHOME ?= /usr/local/javacc-5.0
JAVACC ?= $(JAVACCHOME)/bin/javacc

endif 
# End of win32

######################################## Darwin ##############################
ifeq ($(strip $(OSTYPE)),Darwin)

ifndef MACOSX_DEPLOYMENT_TARGET
$(error "Error: variable MACOSX_DEPLOYMENT_TARGET is not set or exported.")
endif

OPTIMIZE += $(PROFILE)

OSV_MAJOR=$(shell uname -r | awk -F '.' '{print $$1}')
LION_OR_LATER=$(shell [ $(OSV_MAJOR) -ge 11 ] && echo true)
MOUNTAINLION_OR_LATER=$(shell [ $(OSV_MAJOR) -ge 12 ] && echo true)

CCPATH = /usr/bin/

ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.3)
SDK?=/Developer/SDKs/MacOSX10.3.9.sdk
ARCH=-arch ppc
CCPATH = /Developer/usr/bin/
GCC_VERSION=-3.3
CC      = $(CCPATH)gcc$(GCC_VERSION)
CCC     = $(CCPATH)g++$(GCC_VERSION)
endif # 10.3

ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.4)
SDK?=/Developer/SDKs/MacOSX10.4u.sdk
ARCH=-arch i386 -arch ppc
CCPATH = /Developer/usr/bin/
GCC_VERSION=-4.0
CC      = $(CCPATH)gcc$(GCC_VERSION)
CCC     = $(CCPATH)g++$(GCC_VERSION)
endif # 10.4

ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.5)
SDK?=/Developer/SDKs/MacOSX10.5.sdk
ARCH=-arch i386 -arch ppc
CCPATH = /Developer/usr/bin/
GCC_VERSION=-4.2
CC      = $(CCPATH)gcc$(GCC_VERSION)
CCC     = $(CCPATH)g++$(GCC_VERSION)
HASPKGMAKER=1
endif # 10.5

ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.6)
ifdef SDK
SDKLION=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk
ifeq ($(strip $(SDK)),$(SDKLION))
GCC_VERSION=
CCPATH = /Applications/Xcode.app/Contents/Developer/usr/bin/
else
GCC_VERSION?=-4.2
CCPATH = /Developer/usr/bin/
endif
else
SDK?=/Developer/SDKs/MacOSX10.6.sdk
GCC_VERSION?=-4.2
CCPATH = /Developer/usr/bin/
endif
ARCH=-arch x86_64
CC      = $(CCPATH)gcc$(GCC_VERSION)
CCC     = $(CCPATH)g++$(GCC_VERSION)
HASPKGBUILD=1
endif # 10.6

ARCH_DEFINES = -D__darwin__ -DMAJOR_VERSION=$(OSV_MAJOR)
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.3)
ARCH_DEFINES += -B$(SDK)
LNKOPT += -liconv
else
ARCH_DEFINES += -isysroot $(SDK)
LNKOPT += -liconv -Wl,-syslibroot,$(SDK),-headerpad_max_install_names $(ARCH)
endif
LINK_DEBUG=

ifeq ($(strip $(LION_OR_LATER)),true)
SDK?=$(shell xcodebuild -version -sdk | grep SDKs/MacOSX | head -1 | awk -F ' ' '{print $$2}')
ARCH=-arch x86_64 -std=c++11
ifeq ($(strip $(MOUNTAINLION_OR_LATER)),true)
WARNINGS += -Wno-deprecated-register
endif
CCPATH = /usr/bin/
GCC_VERSION=
CC      = $(CCPATH)clang$(GCC_VERSION)
CCC     = $(CCPATH)clang++$(GCC_VERSION)
HASPKGBUILD=1
endif # >= 10.7

OS_NAME=$(strip $(MACOSX_DEPLOYMENT_TARGET))

ifndef NOOPT
OPTIMIZE += -O3 -fPIC
endif # NOOPT

#DEBUG= -g

CXX     = $(CCC)
GCC     = $(CC)

GCCVERSION = $(shell $(CXX) -dumpversion)
GCC_MAJOR = $(word 1, $(subst ., ,$(strip $(GCCVERSION))))
GCC_MINOR = $(word 2, $(subst ., ,$(strip $(GCCVERSION))))

FLEXROOT?=/usr
FLEXINC?=$(FLEXROOT)/include
FLEXLIB?=$(FLEXROOT)/lib
FLEXBIN?=$(FLEXROOT)/bin

READLINE_LIB += -lreadline

LNKOPT += -framework CoreFoundation

STATIC = #-static 
DISTARCH = MacOSX-$(OS_NAME)
DISTCC = gcc$(GCC_VERSION)
STRIP = strip -S

JDKHOME ?= /usr
ANTHOME ?= /usr
JAVACCHOME ?= /usr/local/javacc-5.0
JAVACC ?= $(JAVACCHOME)/bin/javacc

AR=ar
#ARFLAGS=rvs

%_darwin.a: %.a
	rm -rf ./artmp
	mkdir ./artmp
	cd ./artmp; ar -x ../$<
	libtool -static ./artmp/*.o -o $@ -s
	rm -rf ./artmp

libvdm.dylib: libvdm.a
	rm -rf ./artmp
	mkdir ./artmp
	cd ./artmp; ar -x ../$<
	$(CXX) $(ARCH) -dynamiclib -fPIC -o $@ -L. -liconv -Wl,-syslibroot,$(SDK) artmp/*.o
	rm -rf ./artmp

%.dylib: %.a libvdm.dylib
	rm -rf ./artmp
	mkdir ./artmp
	cd ./artmp; ar -x ../$<
	$(CXX) $(ARCH) -dynamiclib -fPIC -o $@ -L. -liconv -Wl,-syslibroot,$(SDK) artmp/*.o -lvdm
	rm -rf ./artmp

endif 
# End of Darwin

##############################################################################
# Experimental: 
# (Compile all files except metaiv.o with -fno-implicit-templates)
TEMPLATE_FLAGS = # -frepo # $(TEMPLATE-$(findstring $@, metaiv.o))
TEMPLATE- = 
TEMPLATE-metaiv.o = # Empty 

LIBLM4H	= vdm_priv.h metaiv.h 
EVALH	= metaiv.h rt_errmsg.h
STATH   = metaiv.h asquotes.h 
CGH     = metaiv.h asquotes.h cgtag.h 
MISCH   = mixedtexvdm.h 

DOCS    = common.y
EXPR    = common.y

all: allsl allpp
allsl: vdmde$(EXE) vdmqt$(EXE) be_spec-sl$(EXE) be_ip-sl$(EXE)
allpp: vppde$(EXE) vppqt$(EXE) be_spec-pp$(EXE) be_ip-pp$(EXE)

all4: vdmde$(EXE) vppde$(EXE) vdmqt$(EXE) vppqt$(EXE)

alltest: vdmde$(EXE) vppde$(EXE) \
	be_spec-sl$(EXE) be_spec-pp$(EXE) be_ip-sl$(EXE) be_ip-pp$(EXE)

# Target: boostrap.
# Used on NT where symbolic links can't be used to build in a local
# directory.
# Instead a local GNUmakefile is put in the local build directory
# with a setting of the VPATH and inclusion of this Makefile.
ifeq ($(strip $(OSTYPE)),win32)
bootstrap:
	@if [ -f GNUmakefile ]; then \
	echo "Will not overwrite GNUmakefile"; exit 1; \
	fi
	echo VPATH='$(SHADOW)' > GNUmakefile
	echo include '$(SHADOW)'/Makefile >> GNUmakefile
#Does not work. Would be useful for perl scripts. 
#echo export PATH:='$(SHADOW)':'$${PATH}' >> GNUmakefile
endif

linkALL: linkTest linkMAKEDEP linkDIVERSE linkPARSER linkLIB linkEVAL \
	linkSS linkCG linkEXC linkRAN linkGUI \
	linkUTILS linkMGR linkMAKE_TOOLS linkPORT linkMISC \
	linkUML linkDEP linkSpecFile linkWIN32 linkINHTREE \
	linkPOG_AUX linkPOG linkPOGPretty 

linkTest: 
	@ EX=0; $(foreach D,${CHECKDIRS}, \
	[ -d ${${D}} ] || { echo Missing dir ${D}=${${D}}; EX=1; } ;\
	)  exit $$EX

linkMISC:
	@ : dummy

FlexLexer.h : $(FLEXROOT)/include/FlexLexer.h
	-$(CP) $(FLEXROOT)/include/FlexLexer.h .

tb_exceptions.o: tb_exceptions.cc tb_exceptions.h

FTANGLE=ftangle
FTANGLE_OPTIONS=-B -\# -Tv -yxb200
# -B = don't beep; -# = ; -Tv = don't be verbose; -yxb = set line length

FWEAVE=fweave
FWEAVE_OPTIONS=-ytw30000 -yxb200 -Tv -B

NUWEB=nuweb
NUWEB_NOTEX=-t
NUWEB_NOCODE=-o
NUWEB_ALWAYS_WRITE=-c  # Without -c nuweb will compare for change
		       # but this is not so usedful on WinNT. 

preprocess.dvi: preprocess.w
	$(NUWEB) $(NUWEB_NOCODE) $<
	latex $(<:.w=.tex)
	$(NUWEB) $(NUWEB_NOCODE) $<
	latex $(<:.w=.tex)

tb_version.dvi: tb_version.w
	$(NUWEB) $(NUWEB_NOCODE) $<
	latex $(<:.w=.tex)
	$(NUWEB) $(NUWEB_NOCODE) $<
	latex $(<:.w=.tex)

vdmpars.web: vdmpars.l
	ln -s $< $@

scanner-sl.cc: vdmpars.web
	$(FTANGLE) $(FTANGLE_OPTIONS) -=tmpscan.l -mVDMSL=1 -mVDMPP=0 -mVICE=0 $<
	$(FLEX) -t tmpscan.l > $@

scanner-pp.cc: vdmpars.web
	$(FTANGLE) $(FTANGLE_OPTIONS) -=tmpscan.l -mVDMSL=0 -mVDMPP=1 -mVICE=0 $<
	$(FLEX) -t tmpscan.l > $@

scanner-rt.cc: vdmpars.web
	$(FTANGLE) $(FTANGLE_OPTIONS) -=tmpscan.l -mVDMSL=0 -mVDMPP=1 -mVICE=1 $<
	$(FLEX) -t tmpscan.l > $@

# Rule for building print_name_vers
ifeq ($(strip $(OSTYPE)),win32)
print_name_vers.exe: $(shell $(LINKCHG) \
	print_name_vers.o tb_version-sl.o date.o libvdm.a)
	$(shell $(LINKCHG) $(CCC) ${LNKOPT} $(DEBUG) $(LINKOUT)$@ $^ $(WINLIBS))
else
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
print_name_vers: print_name_vers.o tb_version-sl.o date.o libvdm.a
else
print_name_vers: print_name_vers.o tb_version-sl.o date.o libvdm_darwin.a
endif
	$(CCC) $(DEBUG) -o $@ $^ $(LNKOPT)
else
print_name_vers: print_name_vers.o tb_version-sl.o date.o libvdm.a
	$(CCC) $(DEBUG) -o $@ $^ $(LNKOPT)
endif
endif

# Rule to create the errmsg.cc file from errmsg.txt
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
convertToHexquad: convertToHexquad.o libvdm.a
else
convertToHexquad: convertToHexquad.o libvdm_darwin.a
endif
	$(CCC) ${LNKOPT} $(DEBUG) -o $@ $^
else
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
convertToHexquad: convertToHexquad.o libvdm.a
	$(CCC) ${LNKOPT} $(DEBUG) -o $@ $^ /usr/lib/libiconv.a
else
convertToHexquad: convertToHexquad.o libvdm.a
	$(CCC) ${LNKOPT} $(DEBUG) -o $@ $^
endif
endif

errmsg.cc: errmsg.txt
ifeq ($(strip $(REALOS)),CYGWIN_NT)
	$(VPATH)/translateErr.pl cc $<
else
	./translateErr.pl cc $<
endif

ifneq ($(strip $(OSTYPE)),win32)
errmsg_jp_hex.txt: errmsg_jp_utf8.txt
	./convertToHexquad $< $@

rt_errmsg_jp.cc: rt_errmsg_jp_utf8.cc
	./convertToHexquad $< $@
endif

errmsg_jp.cc: errmsg_jp_hex.txt
ifeq ($(strip $(REALOS)),CYGWIN_NT)
	$(VPATH)/translateErr2.pl cc $<
else
	./translateErr2.pl cc $<
endif

# Rule to update the statsem error message comments.
.PHONY: updateErrMsg
updateErrMsg:
ifeq ($(strip $(REALOS)),CYGWIN_NT)
	$(VPATH)/replaceComments.pl *.cc
else
	./replaceComments.pl *.cc
endif

LIB_PARSER_COMMON_O=recover.o binops.o preprocess.o contextinfo.o mixedtexvdm.o

LIB_PARSER_SL_O=pexprsl.o parssl.o scanner-sl.o $(LIB_PARSER_COMMON_O)
LIB_PARSER_PP_O=pexprpp.o parspp.o pstmtpp.o scanner-pp.o $(LIB_PARSER_COMMON_O)
LIB_PARSER_RT_O=pexprrt.o parsrt.o pstmtrt.o scanner-rt.o $(LIB_PARSER_COMMON_O)

LIB_PARSER_SL=libparser-sl.a
LIB_PARSER_PP=libparser-pp.a
LIB_PARSER_RT=libparser-rt.a

$(LIB_PARSER_SL): $(LIB_PARSER_SL)($(LIB_PARSER_SL_O))
$(LIB_PARSER_PP): $(LIB_PARSER_PP)($(LIB_PARSER_PP_O))
$(LIB_PARSER_RT): $(LIB_PARSER_RT)($(LIB_PARSER_RT_O))

preprocess.l preprocess.h: preprocess.w
	$(NUWEB) $(NUWEB_ALWAYS_WRITE) $(NUWEB_NOTEX) $<

# Without the option $(NUWEB_ALWAYS_WRITE), nuweb will only update
# the output file if it has changed. Use the .stamp file to ensure
# that everything is kept up to date. 
contextinfo.stamp: contextinfo.w 
	$(NUWEB) $(NUWEB_NOTEX) $<
	touch $@

#These are the output file from contextinfo.w
contextinfo.cc: contextinfo.stamp
contextinfo.h : contextinfo.stamp
contextinfotest.cc: contextinfo.stamp

##contextinfo.cc contextinfo.h contextinfotest.cc: contextinfo.w

contextinfo.dvi: contextinfo.w
	$(NUWEB) $(NUWEB_NOCODE) $<
	latex $(<:.w=.tex)
	$(NUWEB) $(NUWEB_NOCODE) $<
	latex $(<:.w=.tex)

contextinfo.o: contextinfo.cc contextinfo.h 
contextinfotest: contextinfotest.cc contextinfo.o $(VDMLIB)
	$(CXX) -g $^ -o $@

preprocess.cc: preprocess.l
	$(FLEX) -+ -t -Ppp $< > $@

preprocess: preprocess.cc

#############################################################################
# Various object that don't seem to belong anywhere else
#############################################################################

# Object files common to VDMSL and VDMPP:
ALL_OBJ = stringext.o tb_exceptions.o lex.pf_.o

# Object files specific for VDMSL:
ALL_SL_OBJ = vdmde-sl.o cli_options-sl.o \
	astaux-sl.o mangle-sl.o unmangle-sl.o \
	intconvquotes-sl.o \
	tbutils-sl.o tools-sl.o \
	tb_readline-sl.o \
	as2ascii-sl.o \
	tb_version-sl.o \
	$(ALL_GUI_SL_O)

# Object files specific for VDMPP:
ALL_PP_OBJ = vdmde-pp.o cli_options-pp.o \
	astaux-pp.o mangle-pp.o unmangle-pp.o \
	intconvquotes-pp.o \
	tbutils-pp.o tools-pp.o \
	priority.tab-pp.o \
	tb_readline-pp.o \
	as2ascii-pp.o \
	tb_version-pp.o \
	test1-pp.o be-pp.o tm-pp.o \
	vdmexpr-pp.o 

# Object files specific for VICE #
ALL_RT_OBJ = \
        vdmde-rt.o vppexpr-rt.o \
        lex.tp_.o time.o timetrace.o valcond.o \
        tbutils-rt.o tools-rt.o \
	tb_readline-rt.o \
        intconvquotes-rt.o 

#############################################################################
# The Specification File Handler
#############################################################################

##### THE SPECFILE LIBRARY

LIB_SPECFILE_O = specfile.o extr_pp_info.o latexspecfile.o rtfspecfile.o \
                 reader.o text-writer.o tokenscan.o tokenlist.o rtfnewclass.o

LIB_SPECFILE_SL_O= $(LIB_SPECFILE_O)
LIB_SPECFILE_PP_O= $(LIB_SPECFILE_O)
LIB_SPECFILE_RT_O= $(LIB_SPECFILE_O)

LIB_SPECFILE_SL=libspecfile-sl.a
LIB_SPECFILE_PP=libspecfile-pp.a
LIB_SPECFILE_RT=libspecfile-rt.a

$(LIB_SPECFILE_SL): $(LIB_SPECFILE_SL)($(LIB_SPECFILE_SL_O))
$(LIB_SPECFILE_PP): $(LIB_SPECFILE_PP)($(LIB_SPECFILE_PP_O))
$(LIB_SPECFILE_RT): $(LIB_SPECFILE_RT)($(LIB_SPECFILE_RT_O))

##################################################################

ifdef TBVER
MAJORVER = $(word 1, $(subst ., ,$(strip $(TBVER))))
MINORVER = $(word 2, $(subst ., ,$(strip $(TBVER))))
SUBMINORVER = $(word 3, $(subst ., ,$(strip $(TBVER))))
endif

tb_version.h tb_version.cc: tb_version.w
	$(NUWEB) $(NUWEB_ALWAYS_WRITE) $(NUWEB_NOTEX) $<
ifdef TBVER
	cat tb_version.cc | \
	perl -nle 's/#define MAJOR 9/#define MAJOR $(MAJORVER)/; print;' | \
	perl -nle 's/#define MINOR 0/#define MINOR $(MINORVER)/; print;' | \
	perl -nle 's/#define SUBMINOR 0/#define SUBMINOR $(SUBMINORVER)/; print;' > tb_version.tmp
	mv tb_version.tmp tb_version.cc
endif

ifeq ($(strip $(OSTYPE)),Linux)
PROFILING= #-pg  -fno-builtin -fno-inline  #-fno-implicit-templates
DBMALLOC = # -L../../tmp/dmalloc-3.1.2/ # -L../../tmp/dbmalloc_1.14L -ldbmalloc
DBMALLOCLIB = # -ldmalloc
GNULIBPATH = 
endif

# Y A C C  definitions
# Add directive -v to produce *.output for debugging purposes.
# -b NAME uses NAME as prefix instead of y

YACCFILEPREFIX=$(patsubst %.y,%,$(filter %.y,$^))

ifeq ($(strip $(REALOS)),CYGWIN_NT)
ifdef BISON2
FIXPOSASSIGN = $(VPATH)/fixposassign2
else
FIXPOSASSIGN = $(VPATH)/fixposassign
endif
else
ifdef BISON2
FIXPOSASSIGN = ./fixposassign2
else
FIXPOSASSIGN = ./fixposassign
endif
endif

%.cc %.tab.h: %.y 
	${YACC} ${YACCOPT} -p $(YACCFILEPREFIX) -b $(YACCFILEPREFIX) $(YACCFILEPREFIX).y
	./fixerrlab.pl < $(YACCFILEPREFIX).tab.c | $(FIXPOSASSIGN) > $(YACCFILEPREFIX).cc
#	rm -f $(YACCFILEPREFIX).tab.c 

parssl.y: ${DOCS}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -DFULL=1 $(VDMSL) - | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -DFULL=1 $(VDMSL) - | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

parspp.y: ${DOCS}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -DFULL=1  $(VDMPP) - | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -DFULL=1  $(VDMPP) - | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

parsrt.y: ${DOCS}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -DFULL=1 $(VDMRT) - | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -DFULL=1 $(VDMRT) - | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

# Y A C C definitions for expression parser

pexprsl.y: ${EXPR}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL $(VDMSL) -  | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL $(VDMSL) -  | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

pexprpp.y: ${EXPR}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL $(VDMPP) -  | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL $(VDMPP) -  | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

pexprrt.y: ${EXPR}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL $(VDMRT) -  | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL $(VDMRT) -  | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

pstmtpp.y: ${EXPR}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL -DVPPSTMT $(VDMPP) -  | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL -DVPPSTMT $(VDMPP) -  | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

pstmtrt.y: ${EXPR}
	-rm -f $@
ifeq ($(strip $(OSTYPE)),Darwin)
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL -DVPPSTMT $(VDMRT) -  | \
	perl -nle 's:#pragma GCC set_debug_pwd://#pragma GCC set_debug_pwd:; print;' | \
	perl -nle 's/edulcni/#include/; print;' > $@
else
	perl -nle 's/#include/edulcni/; print;' $< | \
	$(PREPROCESS) -UFULL -DVPPSTMT $(VDMRT) -  | \
	perl -nle 's/edulcni/#include/; print;' > $@
endif

####################################################
# Definitions relating to Priority based scheduling#
CFLAGS-pr_test.o = -DVDMPP
CFLAGS-lex.pf_.o = -DVDMPP

priority.tab.cc priority.tab.h:	priority.y SCHDTP-pp.h
	${YACC} -o ./priority.tab.c -d -p pf_ $<
	mv priority.tab.c priority.tab.cc

# End of Definitions relating to Priority based scheduling#
###########################################################

###########################################
# Definitions relating to VICE Timeparser #

CFLAGS-tp_test.o = -DVICE -DVDMPP

timeparser.tab.cc timeparser.tab.h: timeparser.y TIMEMAP.h TIMEPARSER.h
	${YACC} -o ./timeparser.tab.c -d -p tp_ $<
	mv timeparser.tab.c timeparser.tab.cc

TIMEPARSER.o: TIMEPARSER.cc TIMEPARSER.h TIMEPARSER_anonym.cc \
	      TIMEPARSER_anonym.h TIMEPARSER_userdef.h

TIMEMAP.o: TIMEMAP.cc TIMEMAP.h TIMEMAP_anonym.cc \
	   TIMEMAP_anonym.h TIMEMAP_userdef.h

lex.tp_.cc: timeparser.x timeparser.tab.h
	$(FLEX) -t -Ptp_ -B $< > $@

lex.pf_.cc: priority.x priority.tab.h
	$(FLEX) -t -Ppf_ -B $< > $@

TIMETRACETP.o: TIMETRACETP.cc TIMETRACETP.h TIMETRACETP_anonym.cc \
	       TIMETRACETP_anonym.h TIMETRACETP_userdef.h

VC.o: VC.cc VC.h VC_anonym.cc VC_anonym.h VC_userdef.h

# End of definitions relating to VICE Timeparser #
##################################################

## Service Manager Parser

lex.smd_.cc: service_startup.x service_startup.tab.h 
	$(FLEX) -t -Psmd_ -B $< > $@

service_startup.cc service_startup.tab.h: service_startup.y ServiceManagerDesc.h
	${YACC} -p smd_ ${YACCOPT} -b $(YACCFILEPREFIX) $(YACCFILEPREFIX).y 
	mv $(YACCFILEPREFIX).tab.c $(YACCFILEPREFIX).cc

LIB_SERVMAN_O=servicemanager.o service_startup.o \
	      parserservice.o lex.smd_.o \
	      ServiceManagerDesc.o ServiceManagerDesc_anonym.o

LIB_SERVMAN_SL_O=$(LIB_SERVMAN_O)
LIB_SERVMAN_PP_O=$(LIB_SERVMAN_O)
LIB_SERVMAN_RT_O=$(LIB_SERVMAN_O)

LIB_SERVMAN_SL=libservman-sl.a
LIB_SERVMAN_PP=libservman-pp.a
LIB_SERVMAN_RT=libservman-rt.a

$(LIB_SERVMAN_SL): $(LIB_SERVMAN_SL)($(LIB_SERVMAN_SL_O))
$(LIB_SERVMAN_PP): $(LIB_SERVMAN_PP)($(LIB_SERVMAN_PP_O))
$(LIB_SERVMAN_RT): $(LIB_SERVMAN_RT)($(LIB_SERVMAN_RT_O))

# End of Object files specific for VICE #
#########################################

# The VDM++ C++ Classes Library

LIB_VDM_O=metaiv.o m4lib_errmsg.o backslash.o tb_wstring.o \
	vdm_fingerprint.o vdm_recinfomap.o \
	random.o
LIB_VDM=libvdm.a
LIB_VDM_SHARED=libvdm_shared.a
LIB_VDM_FAT=libvdm_fat.a
LIB_VDM_DEBUG=libvdm_debug.a
$(LIB_VDM): $(LIB_VDM)($(LIB_VDM_O))
$(LIB_VDM_SHARED): $(LIB_VDM_SHARED)($(LIB_VDM_O))
$(LIB_VDM_FAT): $(LIB_VDM_FAT)($(LIB_VDM_O))

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
VDMLIB = $(LIB_VDM)
else
VDMLIB = $(LIB_VDM:%.a=%_darwin.a)
endif
else
VDMLIB = $(LIB_VDM)
endif

%_debug.a: 
	rm -rf $*.a $@
	$(MAKE) WINDEBUG=1 $*.a
	mv $*.a $@

LIB_VDM_SO=libvdm.so
$(LIB_VDM_SO): $(LIB_VDM_O)
	$(CXX) -shared -fPIC -o $@ $^ -L.

#
# Library to be used with the generated code.
#
valuescanner.cc: valuescanner.l valueparser.tab.h
	$(FLEX) -t -Pvaluep $< > $@  

valueparser.cc valueparser.tab.h: valueparser.y         
	${YACC} -p valuep ${YACCOPT} -b $(YACCFILEPREFIX) $(YACCFILEPREFIX).y
	mv $(YACCFILEPREFIX).tab.c $(YACCFILEPREFIX).cc 

LIB_CG_O=cg.o cg_aux.o valueparser.o valuescanner.o
LIB_CG=libCG.a
LIB_CG_SHARED=libCG_shared.a
LIB_CG_FAT=libCG_fat.a
LIB_CG_DEBUG=libCG_debug.a
$(LIB_CG): $(LIB_CG)($(LIB_CG_O))
$(LIB_CG_SHARED): $(LIB_CG_SHARED)($(LIB_CG_O))
$(LIB_CG_FAT): $(LIB_CG_FAT)($(LIB_CG_O))

LIB_CG_SO=libCG.so
$(LIB_CG_SO): $(LIB_CG_O)
	$(CXX) -shared -fPIC -o $@ $^ -L. -lvdm

# eval
LIB_EVAL_O= cexpr.o compile.o cpat.o cstmt.o \
            tbdebug.o dlfactory.o \
            evaldl.o evalexpr.o evalsd.o \
	    evalclass.o evaldef.o evalstate.o evalaux.o \
            freevars.o genlatex.o instructions.o libman.o \
            oldname.o pattern.o position.o rt_errmsg.o rt_info.o \
            semvalues.o debug.o stackeval.o \
            val2x.o testcoverage.o vdmstdlib.o settings.o stateutil.o \
	    rt_errmsg_jp.o traces.o

LIB_EVAL_SL_O= evalmod.o $(LIB_EVAL_O)

LIB_EVAL_PP_O= schd.o evalsystem.o $(LIB_EVAL_O) 

LIB_EVAL_RT_O= schd.o evalsystem.o $(LIB_EVAL_O) \
	         time.o timetrace.o valcond.o \
                 timeparser.tab.o lex.tp_.o

LIB_EVAL_SL=libeval-sl.a
LIB_EVAL_PP=libeval-pp.a
LIB_EVAL_RT=libeval-rt.a

$(LIB_EVAL_SL): $(LIB_EVAL_SL)($(LIB_EVAL_SL_O))
$(LIB_EVAL_PP): $(LIB_EVAL_PP)($(LIB_EVAL_PP_O))
$(LIB_EVAL_RT): $(LIB_EVAL_RT)($(LIB_EVAL_RT_O))

#libjava2vdm.a
javaparse.y: javaparse.w
	$(NUWEB) $(NUWEB_ALWAYS_WRITE) $(NUWEB_NOTEX) $<

javascan.cc: javascan.l javaparse.tab.h
	$(FLEX) -t -Pjavas $< > $@

javaparse.cc javaparse.tab.h: javaparse.y
	$(YACC) -pjavap ${YACCOPT} -b $(YACCFILEPREFIX) $(YACCFILEPREFIX).y
	cat $(YACCFILEPREFIX).tab.c | $(FIXPOSASSIGN) > $(YACCFILEPREFIX).cc
	rm -f $(YACCFILEPREFIX).tab.c 

LIB_JAVA2VDM_PP=libjava2vdm-pp.a
LIB_JAVA2VDM_O=javascan.o javaparse.o

LIB_JAVA2VDM_PP_O=$(LIB_JAVA2VDM_O:%.o=%.o)

$(LIB_JAVA2VDM_PP): $(LIB_JAVA2VDM_PP)($(LIB_JAVA2VDM_PP_O))

javamain$(EXE): $(shell $(LINKCHG) \
	priority.tab-pp.o lex.pf_.o \
	javamain-pp.o \
	cli_options-pp.o unmangle-pp.o\
	$(GETOPT_O) astaux-pp.o mangle-pp.o tb_version-pp.o date.o \
	intconvquotes-pp.o \
	tbutils-pp.o tools-pp.o tktreewin-pp.o treecalc.o \
	JSSAUX_userimpl-pp.o \
	libjava2vdm-pp.a \
	libCG-pp.a libss-pp.a libeval-pp.a  \
	stringext.o tb_exceptions.o libservman-pp.a\
	libMGR-pp.a libCG.a libspecfile-pp.a $(WINRES-PP) \
	libjss-pp.a libj2v-pp.a \
	libPOG-pp.a \
	libparser-pp.a libvdm.a libgen-pp.a libcorba-pp.a)
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) \
	)

j2vbe$(EXE): $(shell $(LINKCHG) j2vbe-pp.o as2ascii-pp.o libvdm.a libgen-pp.a libCG.a )
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} -o $@ $+ )

# Unmangling parser
unmangle.cc: unmangle.y
	$(YACC) -punmangle_ ${YACCOPT} -b $(YACCFILEPREFIX) $(YACCFILEPREFIX).y
	mv $(YACCFILEPREFIX).tab.c $(YACCFILEPREFIX).cc

unmangle$(EXE): $(shell $(LINKCHG) unmangle-pp.o libvdm.a libgen-pp.a libCG.a)
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} -o $@ $+ )

# libsssl.a  -- static semantic source files

LIB_SS_O=type.o env.o err.o errmsg.o errmsg_jp.o auxss.o mk.o def.o \
	 expr.o expr2.o expr3.o pat.o stmt.o vcm.o rec.o \
	 AUX_userimpl.o DEF_userimpl.o ENV_userimpl.o \
	 STMT_userimpl.o TYPE_userimpl.o VCM_userimpl.o

LIB_SS_PP_O=$(LIB_SS_O)
LIB_SS_SL_O=$(LIB_SS_O)
LIB_SS_RT_O=$(LIB_SS_O)

LIB_SS_SL=libss-sl.a
LIB_SS_PP=libss-pp.a
LIB_SS_RT=libss-rt.a

$(LIB_SS_SL): $(LIB_SS_SL)($(LIB_SS_SL_O))
$(LIB_SS_PP): $(LIB_SS_PP)($(LIB_SS_PP_O))
$(LIB_SS_RT): $(LIB_SS_RT)($(LIB_SS_RT_O))

# LIB_CG
LIB_CG_COMMON_O= codegen_tools.o backend.o tagmap.o \
	     mod_cgaux.o CGAUX_userimpl.o \
             mod_cgmain.o mod_ds.o mod_stmt.o \
             mod_valdef.o vdmcg.o mod_expr.o mod_fctdef.o \
             mod_patmat.o mod_typedef.o \
             mod_tpgen.o

LIB_CG_SL_O=mod_mod.o mod_statedef.o $(LIB_CG_COMMON_O)
LIB_CG_PP_O=mod_class.o mod_conc.o mod_fvd.o $(LIB_CG_COMMON_O)
LIB_CG_RT_O=mod_class.o mod_conc.o mod_fvd.o $(LIB_CG_COMMON_O)

LIB_CG_SL=libCG-sl.a
LIB_CG_PP=libCG-pp.a
LIB_CG_RT=libCG-rt.a

$(LIB_CG_SL):$(LIB_CG_SL)($(LIB_CG_SL_O))
$(LIB_CG_PP):$(LIB_CG_PP)($(LIB_CG_PP_O))
$(LIB_CG_RT):$(LIB_CG_RT)($(LIB_CG_RT_O))

##############################################################################
# SPECMAN
# The specman is based on C++ genberated from a VDM++ specification
# The global files for the spec man are included in global_cg-xx.cc
# and general definitions in external_cg.h
##############################################################################

LIB_MGR_MODULES = AST NonPrintErr ScriptErr UnitStatus ErrorState Options \
	          ToolColleague VDMUnitElem BaseTools Errors ProjectTypes \
	          ToolKit BatchErr FileStatus PromptErr ToolMediator \
	          CGRepository Interface Repository UpdateSes NoneSes \
	          StructSes FlatSes UpdateProject StatusInfo RepDatabase \
	          Dependency StateStore UMLTool

LIB_MGR_O = $(patsubst %, %.o, $(LIB_MGR_MODULES)) \
	$(patsubst %, %_anonym.o, $(LIB_MGR_MODULES)) \
	CGBase.o projectval.o

LIB_MGR_SL_O=$(LIB_MGR_O:%.o=%.o)
LIB_MGR_PP_O=$(LIB_MGR_O:%.o=%.o)
LIB_MGR_RT_O=$(LIB_MGR_O:%.o=%.o)

LIB_MGR_SL = libMGR-sl.a
LIB_MGR_PP = libMGR-pp.a
LIB_MGR_RT = libMGR-rt.a

$(LIB_MGR_SL): $(LIB_MGR_SL)($(LIB_MGR_SL_O))
$(LIB_MGR_PP): $(LIB_MGR_PP)($(LIB_MGR_PP_O))
$(LIB_MGR_RT): $(LIB_MGR_RT)($(LIB_MGR_RT_O))

##############################################################################
# Common library based on all generated code. The library comes
# in two flavours; libgen-pp.a and libgen-sl.a
##############################################################################

LIB_GEN_MODULES = AS CI REP CPP SEM GLOBAL UTIL INSTRTP DEBUGTP BC BCCI CGTYPES
LIB_GEN_COMMON_O = $(patsubst %, %.o, $(LIB_GEN_MODULES)) \
                   $(patsubst %, %_anonym.o, $(LIB_GEN_MODULES))

LIB_GEN_PP_MODULES = MANGLE SCHDTP
LIB_GEN_PP_COMMON_O = $(patsubst %, %.o, $(LIB_GEN_PP_MODULES)) \
                   $(patsubst %, %_anonym.o, $(LIB_GEN_PP_MODULES))

LIB_GEN_UML_MODULES = DEP ASTMERGE AUML BUILD MERGE UMLERR
LIB_GEN_UML_O = $(patsubst %, %.o, $(LIB_GEN_UML_MODULES)) \
                $(patsubst %, %_anonym.o, $(LIB_GEN_UML_MODULES))

LIB_GEN_SL_O=$(LIB_GEN_COMMON_O:%.o=%-sl.o)

LIB_GEN_PP_O=$(LIB_GEN_COMMON_O:%.o=%-pp.o) \
	     $(LIB_GEN_PP_COMMON_O:%.o=%-pp.o) \
	     $(LIB_GEN_UML_O)

LIB_GEN_RT_O=$(LIB_GEN_COMMON_O:%.o=%-rt.o) \
	     $(LIB_GEN_PP_COMMON_O:%.o=%-rt.o) \
	     $(LIB_GEN_UML_O) \
	     TIMETRACETP.o VC.o TIMEPARSER.o TIMEMAP.o

BUILD.h: BUILD_userimpl.cc

LIB_GEN_SL = libgen-sl.a
LIB_GEN_PP = libgen-pp.a
LIB_GEN_RT = libgen-rt.a

$(LIB_GEN_SL):$(LIB_GEN_SL)($(LIB_GEN_SL_O))
$(LIB_GEN_PP):$(LIB_GEN_PP)($(LIB_GEN_PP_O))
$(LIB_GEN_RT):$(LIB_GEN_RT)($(LIB_GEN_RT_O))

dependGEN: AS-sl.h REP-sl.h AS-pp.h REP-pp.h AUML.h DEP.h UMLERR.h MERGE.h
	$(CXX) -MM -DVDMSL $(CXXFLAGS) $(GENSL) $(GENSLPP:%.cc=%-sl.cc) | \
	perl -p -e 's/^(.*\.o):/$(LIB_GEN_SL)\(\1\):/g' > $(DEPGEN)
	$(CXX) -MM -DVDMPP $(CXXFLAGS) $(GENPP) $(GENSLPP:%.cc=%-pp.cc) | \
	perl -p -e 's/^(.*\.o):/$(LIB_GEN_SL)\(\1\):/g' >> $(DEPGEN)
	$(CXX) -MM -DVDMPP -DVICE $(CXXFLAGS) $(GENPP) $(GENSLPP:%.cc=%-rt.cc) | \
	perl -p -e 's/^(.*\.o):/$(LIB_GEN_SL)\(\1\):/g' >> $(DEPGEN)

DEPGEN = DependGEN

# Two dummy-rules to avoid make from trying to build the target 
# libgen-pp.a() or libgen-sl.a() if either $(GENPP) or $(GENSL) is empty.
#$(GENLIBSL)():
#$(GENLIBPP)():

##############################################################################
# Transforms Library
##############################################################################

LIB_TRANS_MODULES = AEX ASX AX FEX FSX FX OPP SB STRIP XF

LIB_TRANS_O = $(patsubst %, %.o, $(LIB_TRANS_MODULES))\
              $(patsubst %, %_anonym.o, $(LIB_TRANS_MODULES))

LIB_TRANS_PP_O=$(LIB_TRANS_O:%.o=%-pp.o)
LIB_TRANS_RT_O=$(LIB_TRANS_O:%.o=%-pp.o)

LIB_TRANS_PP = libtrans-pp.a
LIB_TRANS_RT = libtrans-rt.a

$(LIB_TRANS_PP):$(LIB_TRANS_PP)($(LIB_TRANS_PP_O))
$(LIB_TRANS_RT):$(LIB_TRANS_RT)($(LIB_TRANS_RT_O))

##############################################################################
# Java2VDM Library
##############################################################################

LIB_J2V_MODULES=J2VAUX J2VBVDM J2VCLASS J2VENV J2VEXPR J2VNS J2VOP \
                J2VSTMT J2VTYPE J2V J2VERR J2VERRMSG

LIB_J2V_O = $(patsubst %, %.o, $(LIB_J2V_MODULES))\
            $(patsubst %, %_anonym.o, $(LIB_J2V_MODULES))

LIB_J2V_PP_O=$(LIB_J2V_O:%.o=%-pp.o) javascan.o javaparse.o javagen_tools.o
LIB_J2V_RT_O=$(LIB_J2V_O:%.o=%-pp.o) javascan.o javaparse.o javagen_tools.o

LIB_J2V_PP = libj2v-pp.a
LIB_J2V_RT = libj2v-rt.a

$(LIB_J2V_PP):$(LIB_J2V_PP)($(LIB_J2V_PP_O))
$(LIB_J2V_RT):$(LIB_J2V_RT)($(LIB_J2V_RT_O))

##############################################################################
# JSS Library
##############################################################################

LIB_JSS_MODULES = JSSAUX JSSDEF JSSENV JSSERRMSG JSSERR JSSEXPR \
                  JSSREP JSSSTMT JSSVCM1 JAVAERR

LIB_JSS_O = $(patsubst %, %.o, $(LIB_JSS_MODULES)) \
            $(patsubst %, %_anonym.o, $(LIB_JSS_MODULES))

LIB_JSS_PP_O=$(LIB_JSS_O:%.o=%-pp.o)
LIB_JSS_RT_O=$(LIB_JSS_O:%.o=%-pp.o)

LIB_JSS_PP = libjss-pp.a
LIB_JSS_RT = libjss-rt.a

$(LIB_JSS_PP):$(LIB_JSS_PP)($(LIB_JSS_PP_O))
$(LIB_JSS_RT):$(LIB_JSS_RT)($(LIB_JSS_RT_O))

###########################################################################
# POG auxiliaries
###########################################################################

LIB_POG_AUX_MODULES = CAST CI_uAUX REP2TYPE SPEC_uAST TYPE2REP PID PURE_uPAT

LIB_POG_AUX_COMMON_O = $(patsubst %, %.o, $(LIB_POG_AUX_MODULES)) \
                       $(patsubst %, %_anonym.o, $(LIB_POG_AUX_MODULES))

LIB_POG_AUX_SL_O = $(LIB_POG_AUX_COMMON_O:%.o=%-sl.o)
LIB_POG_AUX_PP_O = $(LIB_POG_AUX_COMMON_O:%.o=%-pp.o)
LIB_POG_AUX_RT_O = $(LIB_POG_AUX_COMMON_O:%.o=%-rt.o)

LIB_POG_AUX_SL = libPOG_AUX-sl.a
LIB_POG_AUX_PP = libPOG_AUX-pp.a
LIB_POG_AUX_RT = libPOG_AUX-rt.a

$(LIB_POG_AUX_SL): $(LIB_POG_AUX_SL)($(LIB_POG_AUX_SL_O))
$(LIB_POG_AUX_PP): $(LIB_POG_AUX_PP)($(LIB_POG_AUX_PP_O))
$(LIB_POG_AUX_RT): $(LIB_POG_AUX_RT)($(LIB_POG_AUX_RT_O))

###########################################################################
# POG: Proof obligation generator 
###########################################################################

LIB_POG_MODULES = CHECK CON_uENV FRESH GEN_uINV GEN_uPO POGTP POG_uDEF \
	          POG_uEXPR POG_uSTMT POG_uMEDIATOR POG_uPAT

LIB_POG_COMMON_O = $(patsubst %, %.o, $(LIB_POG_MODULES)) \
                   $(patsubst %, %_anonym.o, $(LIB_POG_MODULES))

LIB_POG_SL_O = $(LIB_POG_COMMON_O:%.o=%-sl.o) pog-interface.o
LIB_POG_PP_O = $(LIB_POG_COMMON_O:%.o=%-pp.o) pog-interface.o
LIB_POG_RT_O = $(LIB_POG_COMMON_O:%.o=%-rt.o) pog-interface.o

LIB_POG_SL = libPOG-sl.a
LIB_POG_PP = libPOG-pp.a
LIB_POG_RT = libPOG-rt.a

$(LIB_POG_SL): $(LIB_POG_SL)($(LIB_POG_SL_O))
$(LIB_POG_PP): $(LIB_POG_PP)($(LIB_POG_PP_O))
$(LIB_POG_RT): $(LIB_POG_RT)($(LIB_POG_RT_O))

###########################################################################
# POG pretty printer - AST to text
###########################################################################

LIB_POG_PRETTY_MODULES = PP_uTAR_uDEF PP_uTAR_uEXPR PP_uTAR_uPAT PP_uTAR_uPO \
	                 PP_uTAR_uTYPE PP_uTRA_uDEF PP_uTRA_uEXPR PP_uTRA_uPAT \
	                 PP_uTRA_uPO PP_uTRA_uTYPE PP_uMEDIATOR

LIB_POG_PRETTY_COMMON_O = $(patsubst %, %.o, $(LIB_POG_PRETTY_MODULES)) \
                          $(patsubst %, %_anonym.o, $(LIB_POG_PRETTY_MODULES))

LIB_POG_PRETTY_SL_O = $(LIB_POG_PRETTY_COMMON_O:%.o=%-sl.o)
LIB_POG_PRETTY_PP_O = $(LIB_POG_PRETTY_COMMON_O:%.o=%-pp.o)
LIB_POG_PRETTY_RT_O = $(LIB_POG_PRETTY_COMMON_O:%.o=%-rt.o)

LIB_POG_PRETTY_SL = libPOG_PRETTY-sl.a
LIB_POG_PRETTY_PP = libPOG_PRETTY-pp.a
LIB_POG_PRETTY_RT = libPOG_PRETTY-rt.a

$(LIB_POG_PRETTY_SL): $(LIB_POG_PRETTY_SL)($(LIB_POG_PRETTY_SL_O))
$(LIB_POG_PRETTY_PP): $(LIB_POG_PRETTY_PP)($(LIB_POG_PRETTY_PP_O))
$(LIB_POG_PRETTY_RT): $(LIB_POG_PRETTY_RT)($(LIB_POG_PRETTY_RT_O))

##############################################################################
# UML Tool library
##############################################################################

GENUMLCSRC = 

UMLCODESRC = uml-be.cc mini-p-print.cc vppinterface.cc parse_string.cc \
             roseinterface.cc Ustdafx.cc UmlToolInterface.cc \
             rationalrose.cc rose_aux.cc \
	     xmiinterface.cc xmireader.cc xmiwriter.cc xmiaux.cc xmimodel.cc \
	     XMI_userimpl.cc 

UMLSRC = $(GENUMLCSRC) $(UMLCODESRC)

LIB_UML_O=uml-be.o mini-p-print.o vppinterface.o parse_string.o \
	  UmlToolInterface.o \
	  xmiinterface.o xmireader.o xmiwriter.o xmiaux.o xmimodel.o XMI_userimpl.o

LIB_UML_PP=libUML-pp.a
LIB_UML_RT=libUML-rt.a

$(LIB_UML_PP): $(LIB_UML_PP)($(LIB_UML_O))
$(LIB_UML_RT): $(LIB_UML_RT)($(LIB_UML_O))

ifeq ($(strip $(OSTYPE)),win32)
LIB_UMLROSE_O=$(LIB_UML_O) roseinterface.o rationalrose.o rose_aux.o

LIB_UMLROSE_PP=libUMLROSE-pp.a
LIB_UMLROSE_RT=libUMLROSE-rt.a

$(LIB_UMLROSE_PP): $(LIB_UMLROSE_PP)($(LIB_UMLROSE_O))
$(LIB_UMLROSE_RT): $(LIB_UMLROSE_RT)($(LIB_UMLROSE_O))
endif

# Patched header files have to be generated prior to making dependencies
dependUML: AS-sl.h REP-sl.h AS-pp.h REP-pp.h AUML.h DEP.h UMLERR.h MERGE.h 
	$(CXX) -MM -DVDMPP $(CXXFLAGS) $(UMLSRC) | \
	perl -p -e 's/^(.*\.o):/$(LIB_UML_PP)\(\1\):/g' > $(DEPUML)

DEPUML = DependUML

###
### libxml2
###
ifeq ($(strip $(OSTYPE)),win32)
XMLDIR ?= C:/libxml2
XMLINCL = -I$(XMLDIR)/include -I$(XMLDIR)/include/libxml
XMLLIB = $(XMLDIR)/lib
XMLBIN = $(shell cygpath -u $(XMLDIR))/bin

ifeq ($(strip $(CLCPUTYPE)),x64)
#XML_ARCH_LIB = $(XMLLIB)/libxml2.lib \
#	       $(XMLLIB)/libiconv.lib \
#	       $(XMLLIB)/zlib.lib
XML_ARCH_LIB = $(XMLLIB)/libxml2.lib \
	       $(XMLLIB)/libiconv.lib
#XML_DIST_BIN = $(XMLBIN)/libxml2.dll \
#	       $(XMLBIN)/libiconv.dll \
#	       $(XMLBIN)/zlib1.dll
XML_DIST_BIN = $(XMLBIN)/libxml2.dll \
	       $(XMLBIN)/libiconv.dll
else
XML_ARCH_LIB = $(XMLLIB)/libxml2.lib \
	       $(XMLLIB)/iconv.lib \
	       $(XMLLIB)/zlib.lib
XML_DIST_BIN = $(XMLBIN)/libxml2.dll \
	       $(XMLBIN)/iconv.dll \
	       $(XMLBIN)/zlib1.dll
endif
endif

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
XMLINCL = -I/usr/include/libxml2
XML_ARCH_LIB = -lxml2
endif

ifeq ($(strip $(OSTYPE)),Linux)
XMLINCL = -I/usr/include/libxml2
XML_ARCH_LIB = -lxml2
endif

ifeq ($(strip $(OSTYPE)),Darwin)
XMLINCL = -I$(SDK)/usr/include/libxml2
XML_ARCH_LIB = -lxml2
endif

ifeq ($(strip $(OSTYPE)),SunOS)
XMLINCL = -I/usr/include/libxml2
XML_ARCH_LIB = -lxml2
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
XMLINCL = -I/usr/local/include/libxml2 -I/usr/local/include
XML_ARCH_LIB = -lxml2
endif

XML_LIB = $(XML_ARCH_LIB)

###
### Corba specifics
###

ifeq ($(strip $(USE_CORBA_API)),1)

CORBA_HH = metaiv_idl.hh corba_api.hh internal_api.hh contextinfo.h
LIB_CORBA_O=metaiv_idlSK.o corba_apiSK.o internal_apiSK.o \
	    metaiv_impl.o init_corba.o object_map.o corba_api.o cmd_queue.o

LIB_CORBA_SL=libcorba-sl.a
LIB_CORBA_PP=libcorba-pp.a
LIB_CORBA_RT=libcorba-rt.a

$(LIB_CORBA_SL): $(CORBA_HH) $(LIB_CORBA_SL)($(LIB_CORBA_O))
$(LIB_CORBA_PP): $(CORBA_HH) $(LIB_CORBA_PP)($(LIB_CORBA_O)) 
$(LIB_CORBA_RT): $(CORBA_HH) $(LIB_CORBA_RT)($(LIB_CORBA_O)) 

ifeq ($(strip $(OSTYPE)),win32)
CORBA_API = -DCORBA_API -D_REENTRANT -DDYN_CAST
else
CORBA_API = -DCORBA_API -D_REENTRANT -DDYN_CAST -Wno-overloaded-virtual
endif

#CXXFLAGS += $(CORBA_CPPFLAGS)
CORBAINC = -I$(CORBADIR)/include
CORBA_LIB = $(CORBA_ARCH_LIB)

corba_apiSK.o: metaiv_idlSK.cc metaiv_idl.hh internal_apiSK.cc
corba_api.o: corba_api.h metaiv_impl.h corba_apiSK.cc metaiv_idlSK.cc
metaiv_impl.o: metaiv_impl.h metaiv_idlSK.cc
cmd_queue.o: cmd_queue.cc cmd_queue.h

CFLAGS-libcorba-sl.a += $(CORBAINC) $(CORBA_CPPFLAGS)
CFLAGS-libcorba-pp.a += $(CORBAINC) $(CORBA_CPPFLAGS)
CFLAGS-libcorba-rt.a += $(CORBAINC) $(CORBA_CPPFLAGS)

CFLAGS-libservman-sl.a += $(CORBAINC) $(CORBA_CPPFLAGS)
CFLAGS-libservman-pp.a += $(CORBAINC) $(CORBA_CPPFLAGS)
CFLAGS-libservman-rt.a += $(CORBAINC) $(CORBA_CPPFLAGS)

CFLAGS-tools-sl.o += $(CORBAINC) $(CORBA_CPPFLAGS)
CFLAGS-tools-pp.o += $(CORBAINC) $(CORBA_CPPFLAGS)
CFLAGS-tools-rt.o += $(CORBAINC) $(CORBA_CPPFLAGS)

#
# Win32
#
ifeq ($(strip $(OSTYPE)),win32)
CORBADIR ?= C:/omniORB-4.2.2
CORBADIR_CYGWIN = $(shell cygpath -u $(CORBADIR))
CORBA_CPPFLAGS = -D__WIN32__ -D__x86__ -D_WIN32_WINNT=0x400 -D__NT__ -D__OSVERSION__=4 $(CORBA_API)
CORBALIBDIR = $(CORBADIR)/lib/x86_win32
CORBABINDIR = $(CORBADIR_CYGWIN)/bin/x86_win32

ifeq ($(strip $(MSCVER)),VC8)
VCVER = vc8
else
ifeq ($(strip $(MSCVER)),VC9)
VCVER = vc9
else
ifeq ($(strip $(MSCVER)),VC10)
VCVER = vc10
else
ifeq ($(strip $(MSCVER)),VC11)
VCVER = vc11
else
VCVER = vc12
endif
endif
endif
endif

ifdef WINDEBUG
CORBA_ARCH_LIB = \
	$(CORBALIBDIR)/omniORB422_rtd.lib \
	$(CORBALIBDIR)/omniDynamic422_rtd.lib \
	$(CORBALIBDIR)/omnithread40_rtd.lib \
	wsock32.lib
CORBA_DLL = \
	$(CORBABINDIR)/omniORB422_$(VCVER)_rtd.dll \
	$(CORBABINDIR)/omniDynamic422_$(VCVER)_rtd.dll \
	$(CORBABINDIR)/omnithread40_$(VCVER)_rtd.dll
else
CORBA_ARCH_LIB = \
	$(CORBALIBDIR)/omniORB422_rt.lib \
	$(CORBALIBDIR)/omniDynamic422_rt.lib \
	$(CORBALIBDIR)/omnithread40_rt.lib \
	wsock32.lib
CORBA_DLL = \
	$(CORBABINDIR)/omniORB422_$(VCVER)_rt.dll \
	$(CORBABINDIR)/omniDynamic422_$(VCVER)_rt.dll \
	$(CORBABINDIR)/omnithread40_$(VCVER)_rt.dll
endif

IDLCCFLAGS = -bcxx -Wbs=SK.cc
IDLCC = $(CORBABINDIR)/omniidl
endif

#
# CYGWIN_NT
#
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
CORBADIR = /usr/local/omniORB
CORBALIB_DIR = $(CORBADIR)/lib
CORBALIB = -L$(CORBALIB_DIR)
CORBABIN_DIR = $(CORBADIR)/bin
CORBA_CPPFLAGS = $(CORBA_API)
CORBA_ARCH_LIB = -lomniORB4 -lomnithread -lomniDynamic4
IDLCCFLACS = -bcxx -Wbs=SK.cc 
IDLCC = $(CORBABIN_DIR)/omniidl $(IDLCCFLACS)
CORBA_DIST_BIN = $(shell readlink -f $(CORBABIN_DIR)/cygomniORB4.dll) \
	    $(shell readlink -f $(CORBABIN_DIR)/cygomniDynamic4.dll) \
	    $(shell readlink -f $(CORBABIN_DIR)/cygomnithread.dll)
endif

#
# Linux
#
ifeq ($(strip $(OSTYPE)),Linux)
CORBADIR ?= /usr/local/omniORB
CORBA_ARCH_DIR=
CORBALIB_DIR = $(CORBADIR)/lib
CORBALIB = -L$(CORBALIB_DIR)
CORBABIN_DIR = $(CORBADIR)/bin
CORBA_CPPFLAGS = $(CORBA_API)
CORBA_ARCH_LIB = -lomniORB4 -lomnithread -lomniDynamic4
IDLCCFLACS = -bcxx -Wbs=SK.cc 
IDLCC = $(CORBABIN_DIR)/omniidl $(IDLCCFLACS)
CORBA_DIST_LIB = $(CORBALIB_DIR)/libomniORB4.so.2 \
	    $(CORBALIB_DIR)/libomniDynamic4.so.2 \
	    $(CORBALIB_DIR)/libomnithread.so.4
endif

#
# Darwin
#
ifeq ($(strip $(OSTYPE)),Darwin)
CORBADIR ?= /usr/local/omniORB
CORBALIB_DIR = $(CORBADIR)/lib
CORBABIN_DIR = $(CORBADIR)/bin
CORBALIB = -L$(CORBALIB_DIR)
CORBA_CPPFLAGS = $(CORBA_API)
CORBA_ARCH_LIB = -lomniORB4 -lomnithread -lpthread -lomniDynamic4
LIBOMNIORB4=libomniORB4.2.dylib
LIBOMNIDYNAMIC4=libomniDynamic4.2.dylib
LIBOMNITHREAD=libomnithread.4.dylib
CORBA_DIST_LIB = $(CORBALIB_DIR)/$(LIBOMNIORB4) \
	    $(CORBALIB_DIR)/$(LIBOMNIDYNAMIC4) \
	    $(CORBALIB_DIR)/$(LIBOMNITHREAD)
OLDOMNIORB4=$(CORBALIB_DIR)/$(LIBOMNIORB4)
OLDOMNIDYNAMIC4=$(CORBALIB_DIR)/$(LIBOMNIDYNAMIC4)
OLDOMNITHREAD=$(CORBALIB_DIR)/$(LIBOMNITHREAD)
NEWOMNIORB4=@executable_path/../lib/$(LIBOMNIORB4)
NEWOMNIDYNAMIC4=@executable_path/../lib/$(LIBOMNIDYNAMIC4)
NEWOMNITHREAD=@executable_path/../lib/$(LIBOMNITHREAD)
IDLCCFLACS = -bcxx -Wbs=SK.cc 
IDLCC = $(CORBABIN_DIR)/omniidl $(IDLCCFLACS)
endif

#
# Solaris
#
ifeq ($(strip $(OSTYPE)),SunOS)
CORBADIR ?= /usr/local/omniORB
CORBALIB_DIR = $(CORBADIR)/lib
CORBA_CPPFLAGS = $(CORBA_API)
CORBA_ARCH_LIB = -lomniORB4 -lomnithread -lpthread -lomniDynamic4 -lsocket -lnsl
CORBALIB = -L$(CORBADIR)/lib
IDLCC = $(CORBADIR)/bin/omniidl -bcxx -Wbs=SK.cc
CORBA_DIST_LIB = $(CORBALIB_DIR)/libomniORB4.so.1 \
	    $(CORBALIB_DIR)/libomniDynamic4.so.1 \
	    $(CORBALIB_DIR)/libomnithread.so.3
endif

#
# FreeBSD
#
ifeq ($(strip $(OSTYPE)),FreeBSD)
CORBADIR ?= /usr/local
CORBALIB_DIR = $(CORBADIR)/lib
CORBA_CPPFLAGS = $(CORBA_API)
CORBA_ARCH_LIB = -lomniORB4 -lomnithread -lpthread -lomniDynamic4
CORBALIB = -L$(CORBADIR)/lib
IDLCC = $(CORBADIR)/bin/omniidl -bcxx -Wbs=SK.cc
CORBA_DIST_LIB = $(CORBALIB_DIR)/libomniORB4.so.1 \
	    $(CORBALIB_DIR)/libomniDynamic4.so.1 \
	    $(CORBALIB_DIR)/libomnithread.so.3
endif

endif

%SK.cc %.hh: %.idl
ifeq ($(strip $(OSTYPE)),win32)
	$(CP) $< .
	$(IDLCC)  $(IDLCCFLAGS) $(SHADOWINC) $(notdir $<)
	$(RM) $(notdir $<)
else
	$(IDLCC)  $(IDLCCFLAGS) $<
endif


##
## End of CORBA stuff
##


# Not very nice, but ...
common.y: tbutils.h #ProjectTypes.h projectval.h tools.h
	touch common.y

# date.o  --  Automatic generation of timestamp of LINK-TIME.

.PHONY: PHONY1
PHONY1:
TMPDATENAME = tmp-date.cc
date.o: date.cc
date.cc: PHONY1
	@echo "#include \"date.h\"" > $(TMPDATENAME)
	@echo "#ifdef __cplusplus" >> $(TMPDATENAME)
	@echo "std::wstring date = L\""`LC_TIME=C date +%a\ %d-%h-%Y\ %H:%M:%S\ %z\"\; ` >> $(TMPDATENAME)
	@echo "#else" >> $(TMPDATENAME)
	@echo "wchar_t date[] = L\""`LC_TIME=C date +%a\ %d-%h-%Y\ %H:%M:%S\ %z\"\; ` >> $(TMPDATENAME)
	@echo "#endif" >> $(TMPDATENAME)
	@if  cmp -s date.cc $(TMPDATENAME); then \
          echo $@: no change ; \
        else \
          mv $(TMPDATENAME) $@; \
        fi 

vdmfixproject$(EXE): $(shell $(LINKCHG) vdmfixproject.o libCG.a libvdm.a )
	$(shell $(LINKCHG) \
	  ${CCC} ${LNKOPT} $(LINKOUT)$@ $+ $(LIBMATH))

ifneq ($(strip $(OSTYPE)),win32)
%-sl.cc: %.cc
	-$(LN) $< $@

%-pp.cc: %.cc
	-$(LN) $< $@

%-rt.cc: %.cc
	-$(LN) $< $@
endif

%-sl.c: %.c
	-$(LN) $< $@

%-pp.c: %.c
	-$(LN) $< $@

%.manifest: $(WIN_DIST_BIN)/%.manifest
ifeq ($(strip $(CLCPUTYPE)),x64)
	cat $< | perl -nle 's/x86/amd64/; print;' > $@
else
	-$(LN) $< $@
endif

# Testing dlclasses.

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
dlclass_test1.so: %.so: %.cc libvdm.a
	${CXX} $(ARCH) -dynamiclib -fPIC -o $@ $< -L. -lvdm -liconv -Wl,-syslibroot,$(SDK)
else
dlclass_test1.so: %.so: %.cc libvdm_darwin.a
	${CXX} $(ARCH) -dynamiclib -fPIC -o $@ $< -L. -lvdm_darwin -liconv -Wl,-syslibroot,$(SDK)
endif

dlclass_test1.o: %.o: %.cc
	${CXX} $(ARCH) -fPIC -c -o $@ $< 
endif

ifeq ($(strip $(OSTYPE)),Linux)

dlclass_test1.so: %.so: %.cc libvdm_shared.a
	${CXX} $(ARCH) -shared -fPIC -o $@ $< -L. -lvdm_shared

dlclass_test1.o: %.o: %.cc
	${CXX} $(ARCH) -fPIC -c -o $@ $< 
endif

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)

dlclass_test1.so: %.so: %.cc libvdm.a
	${CXX} $(ARCH) -shared -o $@ $< -L. -lvdm -liconv

dlclass_test1.o: %.o: %.cc
	${CXX} $(ARCH) -c -o $@ $< 
endif

ifeq ($(strip $(OSTYPE)),SunOS)
dlclass_test1.so: %.so: %.cc libvdm_shared.a
	${CXX} $(ARCH) -shared -fPIC -o $@ $< -L. -lvdm_shared -lposix4

dlclass_test1.o: %.o: %.cc
	${CXX} $(ARCH) -fPIC -c -o $@ $< 
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)

dlclass_test1.so: %.so: %.cc libvdm_shared.a
	${CXX} $(ARCH) -shared -fPIC -o $@ $< -L. -lvdm_shared

dlclass_test1.o: %.o: %.cc
	${CXX} $(ARCH) -fPIC -c -o $@ $< 
endif

ifeq ($(strip $(OSTYPE)),win32)
# Static Pattern Rules for building .obj and .dll files. 
dlclass_test1.obj: %.obj: %.cc
	$(DLL_CC) $(DLL_CFLAGS) $(DLLFLAGS) $(DLL_INCPATH) /Fo"$@" $(SRCNAME)$<

dlclass_test1.so: %.so: %.obj
	$(DLL_LINK) $(DLL_LFLAGS) /out:"$@" $(DLL_LPATH) vdm.lib $(DLLWINLIBS) $<
	$(MT) -manifest $@.manifest -outputresource:$@
endif 

#
# j2vdll  (Dynamic link classes for j2v)
#
LIB_J2VDLL_SRC = \
	Character.cc \
	Comparable.cc \
	DriverManager.cc \
	File.cc \
	FileDescriptor.cc \
	FileInputStream.cc \
	FileOutputStream.cc \
	Integer.cc \
	J2VUTIL.cc \
	JavaLocale.cc \
	Number.cc \
	Object.cc \
	OutputStream.cc \
	SqlConnection.cc \
	SqlException.cc \
	SqlStatement.cc \
	SqlResultSet.cc \
	Serializable.cc \
	JavaString.cc \
	StringBuffer.cc \
	StreamTokenizer.cc \
	Double.cc

LIB_J2VDLL_O := $(LIB_J2VDLL_SRC:%.cc=%-j2vdll.o)
LIB_J2VDLL=libj2vdll.a

$(LIB_J2VDLL): $(LIB_J2VDLL)($(LIB_J2VDLL_O))

ifeq ($(strip $(OSTYPE)),win32)

DLL_CC = cl.exe
ifeq ($(strip $(MSCVER)),VC9)
ifdef WINDEBUG
DLL_CFLAGS = /nologo /c /Zi /MDd /W0 /EHsc /TP /D_BIND_TO_CURRENT_VCLIBS_VERSION=1
else
DLL_CFLAGS = /nologo /c /Zi /MD /W0 /EHsc /TP /D_BIND_TO_CURRENT_VCLIBS_VERSION=1
endif
else
ifdef WINDEBUG
DLL_CFLAGS = /nologo /c /Zi /MDd /W0 /EHsc /TP
else
DLL_CFLAGS = /nologo /c /Zi /MD /W0 /EHsc /TP
endif
endif

DLLWINLIBS = $(WINCOMMONLIBS) odbc32.lib odbccp32.lib

DLLFLAGS = /D "_USRDLL" 
DLL_INCPATH  = $(SHADOWINC)
J2VLIB_LOG = /D J2VLIB_LOG
J2VLIB_CFLAGS = $(DLL_CFLAGS) $(J2VLIB_LOG)

DLL_LINK     = link.exe /DLL /nologo
DLL_LPATH    = /LIBPATH:.
DLL_LFLAGS   = /incremental:no /NODEFAULTLIB /MANIFEST

j2vmain.obj: j2vmain.cc 
	$(DLL_CC) $(DLL_CFLAGS) $(DLLFLAGS) $(DLL_INCPATH) $(SRCNAME)$< $(OUTPUT_OPTION)

%-j2vdll.o: %.cc 
	$(DLL_CC) $(DLL_CFLAGS) $(DLL_INCPATH) $(SRCNAME)$< $(OUTPUT_OPTION)

j2vdll.so: $(LIB_J2VDLL_O) j2vmain.obj
	$(DLL_LINK) $(DLL_LFLAGS) /out:j2vdll.so $(DLL_LPATH) vdm.lib $(DLLWINLIBS) $^
	$(MT) -manifest $@.manifest -outputresource:$@
endif # win32

ifeq ($(strip $(OSTYPE)),Darwin)
ODBCDIR= /usr
ODBCINCDIR= -I$(ODBCDIR)/include -I /usr/local/include
ODBCLIBDIR= -L$(ODBCDIR)/lib
ODBCLIB= -liodbc
CXX_FLAGS= -fPIC $(ODBCINCDIR)

%-j2vdll.o: %.cc
	$(CXX) $(ARCH) $(ARCH_DEFINES) $(CXX_FLAGS) -I. -c -o $@ $<

j2vmain.o: j2vmain.cc 
	$(CXX) $(ARCH) $(ARCH_DEFINES) $(CXX_FLAGS) -I. -c -o $@ $<

ifeq ($(strip $(LION_OR_LATER)),true)
j2vdll.so: j2vmain.o $(LIB_J2VDLL_O) libvdm.a
else
j2vdll.so: j2vmain.o $(LIB_J2VDLL_O) libvdm_darwin.a
endif
	$(CXX) $(ARCH) -dynamiclib $(CXX_FLAGS) -o $@ $^ -L. -liconv $(ODBCLIBDIR) $(ODBCLIB) -Wl,-syslibroot,$(SDK)
endif # Darwin

ifeq ($(strip $(OSTYPE)),Linux)
ODBCLIB= -lodbc

CXX_FLAGS=-fPIC

%-j2vdll.o: %.cc
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) -c -o $@ $<

j2vmain.o: j2vmain.cc 
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) -c -o $@ $<

j2vdll.so: j2vmain.o $(LIB_J2VDLL_O) libvdm_shared.a
	$(CXX) -shared $(CXX_FLAGS) -o $@ $^ $(ODBCLIB)
endif # Linux

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
CXX_FLAGS=-D__Cygwin__
ODBCLIB= -lodbc

%-j2vdll.o: %.cc
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) -c -o $@ $<

j2vmain.o: j2vmain.cc 
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) -c -o $@ $<

j2vdll.so: j2vmain.o $(LIB_J2VDLL_O) libvdm.a
	$(CXX) -shared $(CXX_FLAGS) -o $@ $^ $(ODBCLIB) -liconv
endif # Cygwin

ifeq ($(strip $(OSTYPE)),SunOS)
ODBCDIR= /usr/local
ODBCINCDIR= -I$(ODBCDIR)/include
ODBCLIBDIR= -L$(ODBCDIR)/lib
ODBCLIB= -lodbc
ODBC_LIB = $(ODBCLIBDIR)/libodbc.so.1
CXX_FLAGS= -fPIC

%-j2vdll.o: %.cc
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) $(ODBCINCDIR) -c -o $@ $<

j2vmain.o: j2vmain.cc 
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) $(ODBCINCDIR) -c -o $@ $<

j2vdll.so: j2vmain.o $(LIB_J2VDLL_O) libvdm_shared.a
	$(CXX) -shared $(CXX_FLAGS) -o $@ $^ -lposix4  $(ODBCLIBDIR) $(ODBCLIB)
endif # SunOS

ifeq ($(strip $(OSTYPE)),FreeBSD)
ODBCDIR= /usr/local
ODBCINCDIR= -I$(ODBCDIR)/include
ODBCLIBDIR= -L$(ODBCDIR)/lib
ODBCLIB= -lodbc
ODBC_LIB = $(ODBCLIBDIR)/libodbc.so.1
CXX_FLAGS= -fPIC

%-j2vdll.o: %.cc
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) $(ODBCINCDIR) -c -o $@ $<

j2vmain.o: j2vmain.cc 
	$(CXX) $(CXX_FLAGS) -I. $(INCDIR) $(ODBCINCDIR) -c -o $@ $<

j2vdll.so: j2vmain.o $(LIB_J2VDLL_O) libvdm_shared.a
	$(CXX) -shared $(CXX_FLAGS) -o $@ $^ $(ODBCLIBDIR) $(ODBCLIB)
endif # SunOS

# end j2vlib

###############################################################
# The VDM-SL Development Environment
###############################################################

%.lib: lib%.a 
#	cp $< $@
	rm -rf ./artmp
	mkdir ./artmp
	cd ./artmp; ar -x ../$<
	lib /NOLOGO /NODEFAULTLIB /OUT:$@ ./artmp/*.o
	rm -rf ./artmp

%.obj: %.o
	cp $< $@

XX = $(shell sh -c "PATH=.:..:$$PATH; exec xx.sh")

.PHONY: 
xx: $(shell $(LINKCHG) \
         xx.o \
          libvdm.a )
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) \
	)
 
all6cm:
ifeq ($(strip $(OSTYPE)),win32)
	$(MAKE) vdmde.exe vdmqt.exe
	$(MAKE) vppde.exe $(PPGUITOOL)
	$(MAKE) vicede.exe $(RTGUITOOL)
	$(MAKE) be_ip-sl.exe be_spec-sl.exe
	$(MAKE) VDM.jar
else
ifeq ($(strip $(GCC_MAJOR)),3)
	$(MAKE) contextinfo.h tb_version.h
	$(MAKE) NOOPT=1 libCG-sl.a
	$(MAKE) NOOPT=1 libCG-pp.a
	$(MAKE) NOOPT=1 libCG-rt.a
endif
	$(MAKE) libvdm.a
	$(MAKE) vdmde vdmqt
	$(MAKE) vppde vppqt
	$(MAKE) vicede viceqt
endif

ALL_SL_LIBS = $(LIB_POG_SL) $(LIB_POG_PRETTY_SL) $(LIB_POG_AUX_SL) \
	$(LIB_EVAL_SL) $(LIB_CG_SL) $(LIB_SS_SL) \
	$(LIB_SPECFILE_SL) $(LIB_PARSER_SL) \
	$(LIB_MGR_SL) $(LIB_GEN_SL) \
	$(LIB_CORBA_SL) \
	$(LIB_CG) $(LIB_VDM)

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
SL_LIBS = $(ALL_SL_LIBS)
else
SL_LIBS = $(ALL_SL_LIBS:%.a=%_darwin.a)
endif
else
SL_LIBS = $(ALL_SL_LIBS)
endif

ALL_PP_LIBS = $(LIB_POG_PP) $(LIB_POG_PRETTY_PP) $(LIB_POG_AUX_PP) \
	$(LIB_J2V_PP) $(LIB_JSS_PP) $(LIB_TRANS_PP) \
	$(LIB_EVAL_PP) $(LIB_CG_PP) $(LIB_SS_PP) \
	$(LIB_SPECFILE_PP) $(LIB_PARSER_PP) \
	$(LIB_MGR_PP) $(LIB_UML_PP) $(LIB_GEN_PP) \
	$(LIB_CORBA_PP) \
	$(LIB_CG) $(LIB_VDM)

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
PP_LIBS = $(ALL_PP_LIBS)
else
PP_LIBS = $(ALL_PP_LIBS:%.a=%_darwin.a)
endif
else
PP_LIBS = $(ALL_PP_LIBS)

ifeq ($(strip $(OSTYPE)),win32)
PP_ROSE_LIBS = $(LIB_POG_PP) $(LIB_POG_PRETTY_PP) $(LIB_POG_AUX_PP) \
	$(LIB_J2V_PP) $(LIB_JSS_PP) $(LIB_TRANS_PP) \
	$(LIB_EVAL_PP) $(LIB_CG_PP) $(LIB_SS_PP) \
	$(LIB_SPECFILE_PP) $(LIB_PARSER_PP) \
	$(LIB_MGR_PP) $(LIB_UMLROSE_PP) $(LIB_GEN_PP) \
	$(LIB_CORBA_PP) \
	$(LIB_CG) $(LIB_VDM)
endif
endif

ALL_RT_LIBS = $(LIB_POG_RT) $(LIB_POG_PRETTY_RT) $(LIB_POG_AUX_RT) \
	$(LIB_J2V_RT) $(LIB_JSS_RT) $(LIB_TRANS_RT) \
	$(LIB_EVAL_RT) $(LIB_CG_RT) $(LIB_SS_RT) \
	$(LIB_SPECFILE_RT) $(LIB_PARSER_RT) \
	$(LIB_MGR_RT) $(LIB_UML_RT) $(LIB_GEN_RT) \
	$(LIB_CORBA_RT) \
	$(LIB_CG) $(LIB_VDM)

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
RT_LIBS = $(ALL_RT_LIBS)
else
RT_LIBS = $(ALL_RT_LIBS:%.a=%_darwin.a)
endif
else
RT_LIBS = $(ALL_RT_LIBS)

ifeq ($(strip $(OSTYPE)),win32)
RT_ROSE_LIBS = $(LIB_POG_RT) $(LIB_POG_PRETTY_RT) $(LIB_POG_AUX_RT) \ $(LIB_SPECFILE_RT) $(LIB_PARSER_RT) \ $(LIB_J2V_RT) $(LIB_JSS_RT) $(LIB_TRANS_RT) \
	$(LIB_EVAL_RT) $(LIB_CG_RT) $(LIB_SS_RT) \
	$(LIB_MGR_RT) $(LIB_UMLROSE_RT) $(LIB_GEN_RT) \
	$(LIB_CORBA_RT) $(LIB_SERVMAN_RT) \
	$(LIB_CG) $(LIB_VDM)
endif
endif

COMMON_OBJS = $(GETOPT_O) stringext.o tb_exceptions.o date.o

ALL_COMMON_OBJS = cli_options.o astaux.o intconvquotes.o tbutils.o as2ascii.o tb_readline.o

SL_COMMON_OBJS = $(ALL_COMMON_OBJS:%.o=%-sl.o)
SL_OBJS = tb_version-sl.o tools-sl.o $(SL_COMMON_OBJS)

PP_COMMON_OBJS = $(ALL_COMMON_OBJS:%.o=%-pp.o) priority.tab-pp.o lex.pf_.o
PP_OBJS = tb_version-pp.o tools-pp.o $(PP_COMMON_OBJS)

RT_COMMON_OBJS = $(ALL_COMMON_OBJS:%.o=%-rt.o) priority.tab-rt.o lex.pf_.o
RT_OBJS = tb_version-rt.o tools-rt.o $(RT_COMMON_OBJS)

CLI_MAIN_O = vdmde.o
SL_CLI_MAIN_O = $(CLI_MAIN_O:%.o=%-sl.o)
PP_CLI_MAIN_O = $(CLI_MAIN_O:%.o=%-pp.o)
RT_CLI_MAIN_O = $(CLI_MAIN_O:%.o=%-rt.o)

GUI_MAIN_O = vdm.o
SL_GUI_MAIN_O = $(GUI_MAIN_O:%.o=%-sl.o)
PP_GUI_MAIN_O = $(GUI_MAIN_O:%.o=%-pp.o)
RT_GUI_MAIN_O = $(GUI_MAIN_O:%.o=%-rt.o)

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
SL_GUI_LIBS = $(LIB_QTGUI_SL)
PP_GUI_LIBS = $(LIB_QTGUI_PP)
RT_GUI_LIBS = $(LIB_QTGUI_RT)
else
SL_GUI_LIBS = $(LIB_QTGUI_SL:%.a=%_darwin.a)
PP_GUI_LIBS = $(LIB_QTGUI_PP:%.a=%_darwin.a)
RT_GUI_LIBS = $(LIB_QTGUI_RT:%.a=%_darwin.a)
endif
else
SL_GUI_LIBS = $(LIB_QTGUI_SL)
PP_GUI_LIBS = $(LIB_QTGUI_PP)
RT_GUI_LIBS = $(LIB_QTGUI_RT)
ifeq ($(strip $(OSTYPE)),win32)
PP_GUIROSE_LIBS = $(LIB_QTGUIROSE_PP)
RT_GUIROSE_LIBS = $(LIB_QTGUIROSE_RT)
endif
endif

# VDMSL
ifeq ($(strip $(OSTYPE)),win32)
vdmde.exe: $(shell $(LINKCHG) $(WINRES-SL) \
        $(SL_CLI_MAIN_O) $(COMMON_OBJS) $(SL_OBJS) $(SL_LIBS))
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) $(WINCONSOLELINK))
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@

vdmqt.exe: $(shell $(LINKCHG) $(WINRESGDE-SL) \
        $(SL_GUI_MAIN_O) $(COMMON_OBJS) $(SL_OBJS) $(SL_GUI_LIBS) $(SL_LIBS))
	$(shell $(LINKCHG) ${CCC} ${GUILNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBQT) $(CORBA_LIB) $(WINLINK))
	$(MAKE) $(GUIMANIFEST)
	$(MT) -manifest $(GUIMANIFEST) -nologo -outputresource:$@
else
vdmde: $(SL_CLI_MAIN_O) $(COMMON_OBJS) $(SL_OBJS) $(SL_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(READLINE_LIB) $(LNKOPT)

vdmqt: $(SL_GUI_MAIN_O) $(COMMON_OBJS) $(SL_OBJS) $(SL_GUI_LIBS) $(SL_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBQT) $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(READLINE_LIB) $(GUILNKOPT)
endif

# VDM++
ifeq ($(strip $(OSTYPE)),win32)
vppde.exe: $(shell $(LINKCHG) $(WINRES-PP) \
        $(PP_CLI_MAIN_O) $(COMMON_OBJS) $(PP_OBJS) $(PP_LIBS))
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	$+ $(LIBMATH) $(CORBA_LIB) $(XML_LIB) $(WINCONSOLELINK))
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@

vppqt.exe: $(shell $(LINKCHG) $(WINRESGDE-PP) \
        $(PP_GUI_MAIN_O) $(COMMON_OBJS) $(PP_OBJS) $(PP_GUI_LIBS) $(PP_LIBS))
	$(shell $(LINKCHG) $(CCC) ${GUILNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBQT) $(CORBA_LIB) $(XML_LIB) $(WINLINK))
	$(MAKE) $(GUIMANIFEST)
	$(MT) -manifest $(GUIMANIFEST) -nologo -outputresource:$@

vppumlqt.exe: $(shell $(LINKCHG) $(WINRESGDE-PP) \
        $(PP_GUI_MAIN_O) $(COMMON_OBJS) $(PP_OBJS) $(PP_GUIROSE_LIBS) $(PP_ROSE_LIBS))
	$(shell $(LINKCHG) $(CCC) ${GUILNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBQT) $(CORBA_LIB) $(XML_LIB) $(AFXLINK))
	$(MAKE) $(ROSEMANIFEST)
	$(MT) -manifest $(ROSEMANIFEST) -nologo -outputresource:$@
else
vppde: $(PP_CLI_MAIN_O) $(COMMON_OBJS) $(PP_OBJS) $(PP_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(XML_LIB) $(READLINE_LIB) $(LNKOPT)

vppqt: $(PP_GUI_MAIN_O) $(COMMON_OBJS) $(PP_OBJS) $(PP_GUI_LIBS) $(PP_LIBS)
	$(CCC) $(DEBUG) -o $@ $+ $(LIBQT) $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(XML_LIB) $(READLINE_LIB) $(GUILNKOPT)
endif

# VICE
pfparse$(EXE):$(shell $(LINKCHG) \
        priority.tab-pp.o lex.pf_.o pr_test.o\
	libvdm.a libgen-pp.a libCG.a libeval-pp.a)
	$(shell $(LINKCHG) \
          ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	$+ \
        $(LIBMATH)\
	)

# VICE
ifeq ($(strip $(OSTYPE)),win32)
vicede.exe: $(shell $(LINKCHG) $(WINRES-RT) \
        $(RT_CLI_MAIN_O) $(COMMON_OBJS) $(RT_OBJS) $(RT_LIBS))
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(CORBA_LIB) $(XML_LIB) $(WINCONSOLELINK))
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@

viceqt.exe: $(shell $(LINKCHG) $(WINRESGDE-RT) \
        $(RT_GUI_MAIN_O) $(COMMON_OBJS) $(RT_OBJS) $(RT_GUI_LIBS) $(RT_LIBS)) 
	$(shell $(LINKCHG) ${CCC} ${GUILNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBQT) $(CORBA_LIB) $(XML_LIB) $(WINCONSOLELINK))
	$(MAKE) $(GUIMANIFEST)
	$(MT) -manifest $(GUIMANIFEST) -nologo -outputresource:$@

viceumlqt.exe: $(shell $(LINKCHG) $(WINRESGDE-RT) \
        $(RT_GUI_MAIN_O) $(COMMON_OBJS) $(RT_OBJS) $(RT_GUIROSE_LIBS) $(RT_ROSE_LIBS)) 
	$(shell $(LINKCHG) $(CCC) ${GUILNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBQT) $(CORBA_LIB) $(XML_LIB) $(AFXLINK))
	$(MAKE) $(ROSEMANIFEST)
	$(MT) -manifest $(ROSEMANIFEST) -nologo -outputresource:$@
else
vicede: $(RT_CLI_MAIN_O) $(COMMON_OBJS) $(RT_OBJS) $(RT_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(XML_LIB) $(READLINE_LIB) $(LNKOPT)

viceqt: $(RT_GUI_MAIN_O) $(COMMON_OBJS) $(RT_OBJS) $(RT_GUI_LIBS) $(RT_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBQT) $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(XML_LIB) $(READLINE_LIB) $(GUILNKOPT)
endif

tp_test$(EXE): $(shell $(LINKCHG) \
        timeparser.tab.o lex.tp_.o TIMEPARSER.o TIMEMAP.o tp_test.o \
        libvdm.a REP-rt.o libCG.a )
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) \
	)

pr_test$(EXE): $(shell $(LINKCHG) \
        priority.tab.o lex.pf_.o pr_test.o \
        libvdm.a libgen-rt.a libCG.a )
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) \
	)

# Target to help on NT.  On Unix do: make ntinit to update
# automatically built files.

ntinit:
	@if [ "$$OSTYPE" = win32 ]; then \
	  echo $(make) $@ cannot be run on NT. ; exit 1; \
        fi
	$(MAKE) convertToHexquad
	$(MAKE) preprocessed
	$(MAKE) links
	$(MAKE) corba_api.hh internal_api.hh metaiv_idl.hh OSTYPE=`uname`
	$(MAKE) VDM.jar libdoc javaapi.jar ToolboxAPI.jar
	rm -f libvdm.a libvdm_darwin.a

preprocessed: contextinfo.cc contextinfo.h tb_version.cc \
	      unmangle.cc \
	      preprocess.cc \
	      scanner-sl.cc scanner-pp.cc scanner-rt.cc \
	      pexprsl.cc parssl.cc \
	      pexprpp.cc parspp.cc pstmtpp.cc \
	      pexprrt.cc parsrt.cc pstmtrt.cc \
	      javascan.cc javaparse.cc \
	      lex.pf_.cc lex.tp_.cc errmsg.cc errmsg_jp.cc \
	      valueparser.cc valuescanner.cc \
	      service_startup.cc lex.smd_.cc \
	      priority.tab.cc timeparser.tab.cc \
	      java_valueparser/valueparser.stamp \
	      FlexLexer.h rt_errmsg_jp.cc
	      
links:  tb_version-sl.cc tb_version-pp.cc tb_version-rt.cc \
	tools-sl.cc tools-pp.cc tools-rt.cc \
	unmangle-pp.cc unmangle-sl.cc unmangle-rt.cc \
	priority.tab-pp.cc priority.tab-rt.cc

# VDM Graphical Development Environment. 
#
# For dependency generation:
ALL_GUI_O = bitmap_embed.o  
ALL_GUI_SL_O = vdm-sl.o
ALL_GUI_PP_O = vdm-pp.o
ALL_GUI_RT_O = vdm-rt.o

RESGDE=-DGDE
RES-GDETYPE = $(RES$(patsubst vdmg%,GDE,$(filter vdmg%, $@)))

RES-SLTYPE = $(VDM$(patsubst %-sl.res,SL,$(filter %-sl.res, $@)))
RES-PPTYPE = $(VDM$(patsubst %-pp.res,PP,$(filter %-pp.res, $@)))
RES-RTTYPE = $(VDM$(patsubst %-rt.res,RT,$(filter %-rt.res, $@)))
RESFLAG= $(RES-PPTYPE) $(RES-SLTYPE) $(RES-RTTYPE) $(RES-GDETYPE)

%.res: vdmgde.rc
	$(RC) -l 0x409 $(RESFLAG) -fo $@ -r -i. $(subst //,\\\\,$<)

vdmgde-sl.res: vdmgde.rc vdmgde-sl.rch vdmgde.ico
vdmgde-pp.res: vdmgde.rc vdmgde-pp.rch vdmgde.ico
vdmgde-rt.res: vdmgde.rc vdmgde-rt.rch vdmgde.ico

vdmde-sl.res: vdmgde.rc vdmde-sl.rch vdmgde.ico
vdmde-pp.res: vdmgde.rc vdmde-pp.rch vdmgde.ico
vdmde-rt.res: vdmgde.rc vdmde-rt.rch vdmgde.ico

vdmgde-%.rch: print_name_vers.exe
	$< -gui -$* --outputrch > $@

vdmde-%.rch: print_name_vers.exe
	$< -$* --outputrch > $@

##################################################################
# Stand-alone backend for code generator
##################################################################

BE_SPEC_OBJS = test1.o astaux-test.o be.o intconvquotes.o tagmap.o CGTYPES.o

ifeq ($(strip $(OSTYPE)),win32)
be_spec-sl$(EXE): $(shell $(LINKCHG) $(BE_SPEC_OBJS:%.o=%-sl.o) libgen-sl.a libCG.a libvdm.a)
	$(shell $(LINKCHG) $(CCC) ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) $(WINCONSOLELINK) \
	)
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@
else
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
be_spec-sl: $(BE_SPEC_OBJS:%.o=%-sl.o) libgen-sl.a libCG.a libvdm.a
else
be_spec-sl: $(BE_SPEC_OBJS:%.o=%-sl.o) libgen-sl_darwin.a libCG_darwin.a libvdm_darwin.a
endif
	$(CCC) $(DEBUG) -o $@ $+ $(LIBMATH) $(CORBA_LIB) $(LNKOPT)
else
be_spec-sl: $(BE_SPEC_OBJS:%.o=%-sl.o) libgen-sl.a libCG.a libvdm.a
	$(CCC) $(DEBUG) -o $@ $+ $(LIBMATH) $(CORBA_LIB) $(LNKOPT)
endif
endif

ifeq ($(strip $(OSTYPE)),win32)
be_spec-pp$(EXE): $(shell $(LINKCHG) $(BE_SPEC_OBJS:%.o=%-pp.o) libgen-pp.a libCG.a libvdm.a)
	$(shell $(LINKCHG) $(CCC) ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) $(WINCONSOLELINK) \
	)
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@
else
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
be_spec-pp: $(BE_SPEC_OBJS:%.o=%-pp.o) libgen-pp.a libCG.a libvdm.a
else
be_spec-pp: $(BE_SPEC_OBJS:%.o=%-pp.o) libgen-pp_darwin.a libCG_darwin.a libvdm_darwin.a
endif
	$(CCC) $(DEBUG) -o $@ $+ $(LIBMATH) $(CORBA_LIB) $(LNKOPT)
else
be_spec-pp: $(BE_SPEC_OBJS:%.o=%-pp.o) libgen-pp.a libCG.a libvdm.a
	$(CCC) $(DEBUG) -o $@ $+ $(LIBMATH) $(CORBA_LIB) $(LNKOPT)
endif
endif

ifeq ($(strip $(OSTYPE)),win32)
be_spec-rt$(EXE): $(shell $(LINKCHG) $(BE_SPEC_OBJS:%.o=%-rt.o) libgen-rt.a libCG.a libvdm.a)
	$(shell $(LINKCHG) $(CCC) ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) $(WINCONSOLELINK) \
	)
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@
else
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(LION_OR_LATER)),true)
be_spec-rt: $(BE_SPEC_OBJS:%.o=%-rt.o) libgen-rt.a libCG.a libvdm.a
else
be_spec-rt: $(BE_SPEC_OBJS:%.o=%-rt.o) libgen-rt_darwin.a libCG_darwin.a libvdm_darwin.a
endif
	$(CCC) $(DEBUG) -o $@ $+ $(LIBMATH) $(CORBA_LIB) $(LNKOPT)
else
be_spec-rt: $(BE_SPEC_OBJS:%.o=%-rt.o) libgen-rt.a libCG.a libvdm.a
	$(CCC) $(DEBUG) -o $@ $+ $(LIBMATH) $(CORBA_LIB) $(LNKOPT)
endif
endif

be-sl.cc be-pp.cc be-rt.cc: backend.cc
	$(CP) $< $@
	chmod 444 $@

tm-sl.cc tm-pp.cc tm-rt.cc: tagmap.cc
	$(CP) $< $@
	chmod 444 $@

##################################################################
# Stand-alone back-end for the interpreter. Used when testing
# the specification of the dynamic semtantics.
##################################################################

ifeq ($(strip $(OSTYPE)),win32)
be_ip-sl$(EXE): $(shell $(LINKCHG) $(WINRES-SL) \
        be_ip-sl.o parse_expr-sl.o \
	$(COMMON_OBJS) $(SL_OBJS) $(SL_LIBS))
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) $(WINCONSOLELINK) \
	)
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@
else
be_ip-sl: be_ip-sl.o parse_expr-sl.o $(COMMON_OBJS) $(SL_OBJS) $(SL_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(READLINE_LIB) $(LNKOPT)
endif

ifeq ($(strip $(OSTYPE)),win32)
be_ip-pp$(EXE): $(shell $(LINKCHG) $(WINRES-PP) \
        be_ip-pp.o parse_expr-pp.o \
	$(COMMON_OBJS) $(PP_OBJS) $(PP_LIBS))
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	$+ $(LIBMATH) $(CORBA_LIB) $(WINCONSOLELINK) \
	)
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@
else
be_ip-pp: be_ip-pp.o parse_expr-pp.o $(COMMON_OBJS) $(PP_OBJS) $(PP_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(READLINE_LIB) $(LNKOPT)
endif

ifeq ($(strip $(OSTYPE)),win32)
be_ip-rt$(EXE): $(shell $(LINKCHG) $(WINRES-RT) \
        be_ip-rt.o parse_expr-rt.o \
	$(COMMON_OBJS) $(RT_OBJS) $(RT_LIBS))
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	$+ $(LIBMATH) $(CORBA_LIB) $(WINCONSOLELINK) \
	)
	$(MAKE) $(CLIMANIFEST)
	$(MT) -manifest $(CLIMANIFEST) -nologo -outputresource:$@
else
be_ip-rt: be_ip-rt.o parse_expr-rt.o $(COMMON_OBJS) $(RT_OBJS) $(RT_LIBS)
	${CCC} $(DEBUG) -o $@ $+ $(LIBMATH) $(LIB_DL) $(CORBA_LIB) $(READLINE_LIB) $(LNKOPT)
endif

############################################################
# testparser
# Test of TokenSpan generation.
############################################################

testparser-sl$(EXE): $(shell $(LINKCHG) \
         testparser-sl.o \
         $(GETOPT_O) astaux-sl.o\
	 intconvquotes-sl.o \
	 tbutils-sl.o tools-sl.o libspecfile-sl.a tb_version-sl.o date.o \
	 libCG-sl.a libss-sl.a libeval-sl.a libparser-sl.a \
	 stringext.o tb_exceptions.o \
         libMGR-sl.a libCG.a libvdm.a libgen-sl.a $(WINRES-SL) )
	$(shell $(LINKCHG) ${CCC} ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) \
	)

######################################################################
# Backend to pretty print and AST
######################################################################
pp-ast-sl: $(shell $(LINKCHG) pp-ast-sl.o mini-p-print-sl.o parse_expr-sl.o \
	astaux-sl.o be-sl.o tm-sl.o\
	intconvquotes-sl.o \
	tbutils-sl.o tools-sl.o tktreewin-sl.o treecalc.o \
        libCG-sl.a libss-sl.a libeval-sl.a  \
        stringext.o tb_exceptions.o \
        libMGR-sl.a libspecfile-sl.a \
        libparser-sl.a libgen-sl.a  libvdm.a libCG.a $(LIB_CORBA_SL) $(WINRES-SL))
	$(shell $(LINKCHG) $(CCC) ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) \
	)
pp-ast-pp: $(shell $(LINKCHG) pp-ast-pp.o mini-p-print-pp.o parse_expr-pp.o \
	astaux-pp.o be-pp.o tm-pp.o\
	intconvquotes-pp.o \
	tbutils-pp.o tools-pp.o tktreewin-pp.o treecalc.o \
        libCG-pp.a libss-pp.a libeval-pp.a  \
        stringext.o tb_exceptions.o \
        libMGR-pp.a libspecfile-pp.a \
        libparser-pp.a libgen-pp.a  libvdm.a libCG.a $(LIB_CORBA_PP) $(WINRES-SL))
	$(shell $(LINKCHG) $(CCC) ${LNKOPT} $(DEBUG) $(LINKOUT)$@ \
	  $+ $(LIBMATH) $(CORBA_LIB) \
	)
# files automatically generated from the spec


#FILTER=$(PWD)/baselink/make_tools/ifdeffilt
FILTER="filepp -w -lc \\\\\\\\\\\\"

gencode gencodeclean specclean:
ifeq ($(strip $(OSTYPE)),Linux)
	$(MAKE) LANG=en_US.utf8 IFDEFFILT=$(FILTER) -f Makefile.gencode $@
else
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
	$(MAKE) LANG=en_US.UTF-8 IFDEFFILT=$(FILTER) VDMCGEXT=.cc -f Makefile.gencode $@
else
	$(MAKE) LANG=en_US.UTF-8 IFDEFFILT=$(FILTER) -f Makefile.gencode $@
endif
endif

##############################################################################
#                               DEPENDENCY SECTION
##############################################################################
# Target in this section:
# 	
# make [OS=NT] dependALL 
#       creates (overwrites) the dependency file in
#       DependALL.$(OSTYPE) (symlink to
#       BASEDIR/build/DependALL.$(OSTYPE). This file is included in
#       this makefile. It is under version control so you can use cvs
#       diff in order to check dependency changes. Dependency for NT
#       is created on hermes with the 'OS=NT' flag.
#
##############################################################################

DEPEND_ALL_BASE = dependALL
.phony: $(DEPEND_ALL_BASE) 

DEPFILE = DependALL.$(OSTYPE)
DEPFILE_TMP = $(DEPFILE).tmp

# Dependency on NT should be created on hermes with proper
# settings of flags:
ifeq ($(strip $(OS)),NT)
OSTYPE=win32
CXXFLAGS += -D_MSC_VER -D__MSDOS__ -DFLM -U__GNUC__ -U__alpha -DCORBA_API -ULINUX -U__linux__
endif

# Sources to library object files:
ALL_LIB_SRC = $(sort $(LIB_EXC_EMUL_O:%.o=%.cc) \
              $(LIB_PARSER_PP_O:%.o=%.cc) \
	      $(LIB_PARSER_SL_O:%.o=%.cc) \
	      $(LIB_VDM_O:%.o=%.cc) $(LIB_CG_O:%.o=%.cc) \
	      $(LIB_EVAL_O:%.o=%.cc) $(LIB_EVAL_PP_O:%.o=%.cc) \
	      $(LIB_SS_O:%.o=%.cc) $(LIB_MGR_O:%.o=%.cc) \
              $(GENSLPP:%.cc=%-sl.cc) \
	      $(GENSLPP:%.cc=%-pp.cc) $(GENSLPP:%.cc=%-rt.cc) \
	      $(GENSL) $(GENPP) $(UMLSRC) \
	      $(LIB_CG_SL_O:%.o=%.cc) $(LIB_CG_PP_O:%.o=%.cc) \
	      $(LIB_SPECFILE_PP_O:%.o=%.cc) $(LIB_SPECFILE_SL_O:%.o=%.cc) \
              $(LIB_SERVMAN_PP_O:%.o=%.cc) \
              $(LIB_QTGUI_O:%.o=%.cc)) \
              $(LIB_GEN_RT_O:%.o=%.cc) \
              $(LIB_PARSER_RT_O:%.o=%.cc) \
              $(LIB_EVAL_RT_O:%.o=%.cc) \
              $(UMLVICESRC) \
              $(LIB_CG_RT_O:%.o=%.cc)) \
              $(LIB_SPECFILE_RT_O:%.o=%.cc) \
              $(LIB_SPECFILE_RTUML_O:%.o=%.cc) \
	      $(LIB_GEN_PP_O:%.o=%.cc) $(LIB_GEN_SL_O:%.o=%.cc)

DEP_PL = depend.pl
CMM = $(CXX) -MM $(CXXFLAGS)

dep_preprocess: preprocess.h 

# The DEPEND target is also dependent of certain files created by 
# preprocessing and/or patching:
# Note that we output to DEPFILE_TMP to avoid DEPFILE to be partially 
# created. 
#  patchCGGEN 

$(DEPEND_ALL_BASE): $(ALL_LIB_SRC) $(ALL_OBJ:%.o=%.cc) $(ALL_SL_OBJ:%.o=%.cc) \
                    $(ALL_PP_OBJ:%.o=%.cc) $(ALL_GUI_PP_O:%.o=%.cc) \
                    $(ALL_RT_OBJ:%.o=%.cc) \
                    tb_version.h tb_version.cc getopt.c\
                    AS-sl.h REP-sl.h AS-pp.h REP-pp.h AUML.h DEP.h UMLERR.h \
                    $(GENMAP) dep_preprocess
#	-$(CMM) $(LIB_RANDOM_O:%.o=%.cc) | $(DEP_PL) $(LIB_RANDOM) > $(DEPFILE_TMP)
	-$(CMM) $(LIB_EXC_EMUL_O:%.o=%.cc)|$(DEP_PL) $(LIB_EXC_EMUL) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_PARSER_PP_O:%.o=%.cc)|$(DEP_PL) $(LIB_PARSER_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVICE -DVDMPP $(LIB_PARSER_RT_O:%.o=%.cc)|$(DEP_PL) $(LIB_PARSER_RT) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(LIB_PARSER_SL_O:%.o=%.cc)|$(DEP_PL) $(LIB_PARSER_SL) >> $(DEPFILE_TMP)
	-$(CMM) $(LIB_VDM_O:%.o=%.cc) |$(DEP_PL) $(LIB_VDM) >> $(DEPFILE_TMP)
	-$(CMM) $(LIB_CG_O:%.o=%.cc) | $(DEP_PL) $(LIB_CG) >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(LIB_EVAL_RT_O:%.o=%.cc) |$(DEP_PL) $(LIB_EVAL_RT) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_EVAL_PP_O:%.o=%.cc) |$(DEP_PL) $(LIB_EVAL_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(LIB_EVAL_SL_O:%.o=%.cc) |$(DEP_PL) $(LIB_EVAL_SL) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(LIB_SS_SL_O:%.o=%.cc) |$(DEP_PL) $(LIB_SS_SL) >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(LIB_SS_RT_O:%.o=%.cc) |$(DEP_PL) $(LIB_SS_RT) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_SS_PP_O:%.o=%.cc) |$(DEP_PL) $(LIB_SS_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(LIB_MGR_SL_O:%.o=%.cc) |$(DEP_PL) $(LIB_MGR_SL) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_MGR_PP_O:%.o=%.cc) |$(DEP_PL) $(LIB_MGR_PP) >> $(DEPFILE_TMP)
#	-$(CMM) -DVDMPP -DVICE $(LIB_MGR_O:%.o=%.cc) |$(DEP_PL) $(LIB_MGR_RT) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(MGRSLPP:%.cc=%-sl.cc)|$(DEP_PL) $(LIB_MGR_SL) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(MGRSLPP:%.cc=%-pp.cc) | \
#	perl -p -e 's/^(.*\.o):/$(LIB_MGR_PP)\(\1\) $(MGRLIBUML)\(\1\):/g' >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(MGRSLPP:%.cc=%-rt.cc) | \
#	perl -p -e 's/^(.*\.o):/$(LIB_MGR_RT)\(\1\) $(LIB_MGR_RTUML)\(\1\):/g' >> $(DEPFILE_TMP)
#	-$(CMM) -DVDMSL $(GENSL) $(GENSLPP:%.cc=%-sl.cc) | $(DEP_PL) $(GENLIBSL) >> $(DEPFILE_TMP)
#	-$(CMM) -DVDMPP $(GENPP) $(GENSLPP:%.cc=%-pp.cc) | $(DEP_PL) $(GENLIBPP) >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(GENPP) $(GENSLPP:%.cc=%-rt.cc) | $(DEP_PL) $(GENLIBPP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(LIB_CG_SL_O:%.o=%.cc)|$(DEP_PL) $(LIB_CG_SL) >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(LIB_CG_RT_O:%.o=%.cc)|$(DEP_PL) $(LIB_CG_RT) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_CG_PP_O:%.o=%.cc)|$(DEP_PL) $(LIB_CG_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(LIB_SPECFILE_SL_O:%.o=%.cc)|$(DEP_PL) $(LIB_SPECFILE_SL) >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(LIB_SPECFILE_RT_O:%.o=%.cc)|$(DEP_PL) $(LIB_SPECFILE_RT) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_SPECFILE_PP_O:%.o=%.cc)|$(DEP_PL) $(LIB_SPECFILE_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(LIB_GEN_SL_O:%.o=%.cc)|$(DEP_PL) $(LIB_GEN_SL) >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(LIB_GEN_RT_O:%.o=%.cc)|$(DEP_PL) $(LIB_GEN_RT) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_GEN_PP_O:%.o=%.cc)|$(DEP_PL) $(LIB_GEN_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_SERVMAN_PP_O:%.o=%.cc)|$(DEP_PL) $(LIB_SERVMAN_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_QTGUI_O:%.o=%.cc)|$(DEP_PL) $(LIB_QTGUI_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_UML_O:%.o=%.cc)|$(DEP_PL) $(LIB_UML_PP) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(LIB_UML_NOROSE_O:%.o=%.cc)|$(DEP_PL) $(LIB_UML_NOROSE_PP) >> $(DEPFILE_TMP)
	-$(CMM) $(ALL_OBJ:%.o=%.cc) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMSL $(ALL_SL_OBJ:%.o=%.cc) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(ALL_PP_OBJ:%.o=%.cc) >> $(DEPFILE_TMP)
	-$(CMM) -DVDMPP $(ALL_GUI_PP_OBJ:%.o=%.cc) >> $(DEPFILE_TMP)
#	-$(CMM) -DVICE -DVDMPP $(ALL_RT_OBJ:%.o=%.cc) >> $(DEPFILE_TMP)
	cat $(DEPFILE_TMP) >$(DEPFILE) && rm $(DEPFILE_TMP)

# Dont try to remake the dependency file:
$(DEPFILE)::

ifdef VPATH
include $(VPATH)/$(DEPFILE)
else
include $(DEPFILE)
endif

############################## END DEPENDENCY SECTION ##########################

# Common distribution stuff shared between unix and win32

SL_EXAMPLES_DIRS = alarm bank cgrel general lang sort sortops worldcup
#PP_EXAMPLES_DIRS = bank os sort sort2 stack tree vm webserver worldcup dl-example
PP_EXAMPLES_DIRS = bank os sort sort2 stack tree webserver worldcup dl-example

inst-sl-examples: examples
	mkdir -p $(DISTDIR)/examples
	for i in $(SL_EXAMPLES_DIRS); do echo $$i; cp -r $</sl/$$i $(DISTDIR)/examples; done
ifeq ($(strip $(OSTYPE)),win32)
	sleep 5 
endif
	find $(DISTDIR)/examples -name CVS -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \.cvsignore -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \.svn -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \*~ -print | xargs rm -f  
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
	mv $(DISTDIR)/examples/sort/Makefile.cygwin $(DISTDIR)/examples/sort/Makefile
endif

inst-pp-examples: examples
	mkdir -p $(DISTDIR)/examples
	for i in $(PP_EXAMPLES_DIRS); do echo $$i; cp -r $</pp/$$i $(DISTDIR)/examples; done
ifeq ($(strip $(OSTYPE)),win32)
	sleep 5 
endif
	find $(DISTDIR)/examples -name CVS -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \.cvsignore -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \.svn -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \*~ -print | xargs rm -f  
ifeq ($(strip $(OSTYPE)),win32)
	rm -rf $(DISTDIR)/examples/dl-example/bigint_unix.vpp
ifeq ($(strip $(CLCPUTYPE)),x64)
	mv $(DISTDIR)/examples/dl-example/mapm_3.60/mapm.lib $(DISTDIR)/examples/dl-example/mapm_3.60/mapm_x86.lib
	cp $(DISTDIR)/examples/dl-example/mapm_3.60/mapm_x64.lib $(DISTDIR)/examples/dl-example/mapm_3.60/mapm.lib
endif
else
	cp $(DISTDIR)/examples/dl-example/bigint_unix.vpp $(DISTDIR)/examples/dl-example/bigint.vpp
endif
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
	mv $(DISTDIR)/examples/sort/Makefile.cygwin $(DISTDIR)/examples/sort/Makefile
	mv $(DISTDIR)/examples/dl-example/Makefile.cygwin $(DISTDIR)/examples/dl-example/Makefile
endif

inst-rt-examples: examples
	mkdir -p $(DISTDIR)/examples
	for i in $(PP_EXAMPLES_DIRS); do echo $$i; cp -r $</pp/$$i $(DISTDIR)/examples; done
ifeq ($(strip $(OSTYPE)),win32)
	sleep 5 
endif
	find $(DISTDIR)/examples -name CVS -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \.cvsignore -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \.svn -print | xargs rm -rf  
	find $(DISTDIR)/examples -name \*~ -print | xargs rm -f  
ifeq ($(strip $(OSTYPE)),win32)
	rm -rf $(DISTDIR)/examples/dl-example/bigint_unix.vpp
else
	cp $(DISTDIR)/examples/dl-example/bigint_unix.vpp $(DISTDIR)/examples/dl-example/bigint.vpp
endif
	cat $(DISTDIR)/examples/dl-example/Makefile | \
	perl -nle 's/vppde/vicede/; print;' > \
	$(DISTDIR)/examples/dl-example/Makefile.temp
	mv $(DISTDIR)/examples/dl-example/Makefile.temp $(DISTDIR)/examples/dl-example/Makefile
	cat $(DISTDIR)/examples/sort/Makefile | \
	perl -nle 's/vppde/vicede/; print;' > \
	$(DISTDIR)/examples/sort/Makefile.temp
	mv $(DISTDIR)/examples/sort/Makefile.temp $(DISTDIR)/examples/sort/Makefile

# Java library

#VDM.jar libdoc javadoc javadist cleanjavadist \
#java_valueparser/valueparser.stamp: Makefile.javalib
#	$(MAKE) VPATH=$(VPATH) JDKHOME=$(JDKHOME) JAVACC=$(JAVACC) -f $< $@ 

VDM.jar javadoc javadist cleanjavadist \
java_valueparser/valueparser.stamp: Makefile.javalib ValueParser.jj
	$(MAKE) VPATH=$(VPATH) JDKHOME=$(JDKHOME) JAVACC=$(JAVACC) -f $< $@ 

libdoc: Makefile.javalib
	$(MAKE) VPATH=$(VPATH) JDKHOME=$(JDKHOME) JAVACC=$(JAVACC) -f $< javadoc

ifeq ($(strip $(OSTYPE)),win32)
JAVA-EXAMPLES-DIR = $(VPATH)/examples/pp/javasort
JAVA-STDLIB = $(VPATH)/IO.java $(VPATH)/MATH.java $(VPATH)/VDMUtil.java
JAVA-STDLIB_QUOTES = $(VPATH)/quotes/start.java $(VPATH)/quotes/append.java
JAVAAPIJAR = $(VPATH)/speclink/java2vdm/javaapi/lib
JAVA2VDM-API = $(VPATH)/speclink/java2vdm/javaapi
JAVA2VDM-EXAMPLES = $(VPATH)/examples/j2v-examples
JAVA2VDM-APILIB = $(VPATH)/javaapi.jar
else
JAVA-EXAMPLES-DIR = examples/pp/javasort
JAVA-STDLIB = IO.java MATH.java VDMUtil.java
JAVA-STDLIB_QUOTES = quotes/start.java quotes/append.java
JAVAAPIJAR = speclink/java2vdm/javaapi/lib
JAVA2VDM-API = speclink/java2vdm/javaapi
JAVA2VDM-EXAMPLES = examples/j2v-examples
JAVA2VDM-APILIB = javaapi.jar
endif

WORKPATH = $(shell pwd)
CLASSPATH = $(WORKPATH)/VDM.jar

javaapi.jar: Makefile.javaapijar
	$(MAKE) ANTHOME=$(ANTHOME) CLASSPATH=$(CLASSPATH) -C $(JAVAAPIJAR) -f $< $@
	cp $(JAVAAPIJAR)/dist/javaapi.jar javaapi.jar 

cleanjavaapi: Makefile.javaapijar
	$(MAKE) ANTHOME=$(ANTHOME) CLASSPATH=$(CLASSPATH) -C $(JAVAAPIJAR) -f $< $@

# win32 distribution.

inst-java2vdm: $(JAVA2VDM-API)/java $(JAVA2VDM-API)/vpp $(JAVA2VDM-EXAMPLES) $(JAVA2VDM-APILIB)
	rm -rf $(DISTDIR)/java2vdm
	mkdir -p $(DISTDIR)/java2vdm
	mkdir -p $(DISTDIR)/java2vdm/javaapi
	cp -r $(JAVA2VDM-API)/java $(DISTDIR)/java2vdm/javaapi
	cp -r $(JAVA2VDM-API)/vpp $(DISTDIR)/java2vdm/javaapi
	cp -r $(JAVA2VDM-EXAMPLES) $(DISTDIR)/java2vdm
	cp -r $(JAVA2VDM-APILIB) $(DISTDIR)/java2vdm
ifeq ($(strip $(OSTYPE)),win32)
	sleep 5 
endif
	mv $(DISTDIR)/java2vdm/j2v-examples $(DISTDIR)/java2vdm/examples
	find $(DISTDIR)/java2vdm -name CVS -print | xargs rm -rf  
	find $(DISTDIR)/java2vdm -name \.cvsignore -print | xargs rm -rf  
	find $(DISTDIR)/java2vdm -name \.svn -print | xargs rm -rf  
	find $(DISTDIR)/java2vdm -type f -print | xargs chmod ugo-x 

inst-javacg: VDM.jar libdoc $(JAVA-EXAMPLES-DIR) $(JAVA-STDLIB) $(JAVA-STDLIB_QUOTES)
	mkdir -p $(DISTDIR)/javacg
	mkdir -p $(DISTDIR)/javacg/quotes
ifeq ($(strip $(OSTYPE)),win32)
	cp -r $^ $(DISTDIR)/javacg/
else
	cp -R -L $^ $(DISTDIR)/javacg/
endif
	mv $(DISTDIR)/javacg/start.java $(DISTDIR)/javacg/quotes/.
	mv $(DISTDIR)/javacg/append.java $(DISTDIR)/javacg/quotes/.
	find $(DISTDIR)/javacg -name CVS -print | xargs rm -rf  
	find $(DISTDIR)/javacg -name \.cvsignore -print | xargs rm -rf  
	find $(DISTDIR)/javacg -name \.svn -print | xargs rm -rf  
ifeq ($(strip $(OSTYPE)),win32)
	cp $(VPATH)/VDM.jar .
endif

inst-qt: $(QT_IMG)
ifdef QT_IMAGE_FILE
	cp -R -L $< $(DISTDIR)/.
	find $(DISTDIR)/$(QT_IMG) -name CVS -print | xargs rm -rf  
	find $(DISTDIR)/$(QT_IMG) -name \.svn -print | xargs rm -rf  
endif

inst-word: VDM.dot
	mkdir -p $(DISTDIR)/word
	$(LN) $(patsubst %,$(WORDDIR)/%, $(WORDFILES)) $(DISTDIR)/word/

inst-sl-emacs: $(EMACSSLFILES)
	mkdir -p $(DISTDIR)/emacs
	$(LN) $(patsubst %,$(EMACSDIR)/%, $(EMACSSLFILES)) $(DISTDIR)/emacs/

inst-rt-emacs: $(EMACSRTFILES)
	mkdir -p $(DISTDIR)/emacs
	$(LN) $(patsubst %,$(EMACSDIR)/%, $(EMACSRTFILES)) $(DISTDIR)/emacs/

inst-pp-emacs: $(EMACSPPFILES)
	mkdir -p $(DISTDIR)/emacs
	$(LN) $(patsubst %,$(EMACSDIR)/%, $(EMACSPPFILES)) $(DISTDIR)/emacs/

inst-sl-latex: $(LATEXSLFILES)
	mkdir -p $(DISTDIR)/latex
	$(LN) $(patsubst %,$(LATEXDIR)/%, $(LATEXSLFILES)) $(DISTDIR)/latex/

inst-pp-latex: $(LATEXPPFILES)
	mkdir -p $(DISTDIR)/latex
	$(LN) $(patsubst %,$(LATEXDIR)/%, $(LATEXPPFILES)) $(DISTDIR)/latex/

inst-sl-stdlib: 
	mkdir -p $(DISTDIR)/stdlib
	$(LN) $(patsubst %,$(STDLIBDIR)/%, $(STDLIB_FILES_SL)) $(DISTDIR)/stdlib/

inst-pp-stdlib: 
	mkdir -p $(DISTDIR)/stdlib
	$(LN) $(patsubst %,$(STDLIBDIR)/%, $(STDLIB_FILES_PP)) $(DISTDIR)/stdlib/

ifeq ($(strip $(OSTYPE)),win32)
POWERTEST_DIR = $(HOME)/powertest
POWERTEST_CGFILES_DIR = $(POWERTEST_DIR)/cgfiles
inst-powertest-files:
	mkdir -p $(POWERTEST_CGFILES_DIR)
	cp $(patsubst %,$(VPATH)/%, $(CG_FILES_INCLUDES)) $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/IO_userimpl-*.cc $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/MATH_userimpl-*.cc $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/VDMUtil_userimpl-*.cc $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/IO_userimpl.cpp $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/MATH_userimpl.cpp $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/VDMUtil_userimpl.cpp $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/IO.java $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/MATH.java $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/VDMUtil.java $(POWERTEST_CGFILES_DIR)
	cp $(VPATH)/VDM.jar $(POWERTEST_CGFILES_DIR)

NSISSCR = vdmtools.nsi
ifeq ($(strip $(CLCPUTYPE)),x64)
WISE = "c:/Program Files (x86)/Wise Installation System/Wise32" 
MAKENSIS = "c:/Program Files (x86)/NSIS/makensis"
WIN64 = /DWIN64=1
else
WISE = "c:/Program Files/Wise Installation System/Wise32" 
MAKENSIS = "c:/Program Files/NSIS/makensis"
WIN64 =
endif

WISEDEF-sl  = -D _VDMSL_=1
WISEDEF-pp  = -D _VDMSL_=0
WISEDEF-rt  = -D _VDMSL_=0

INSTVERSION-sl  = sl
INSTVERSION-pp  = pp
INSTVERSION-rt  = rt

NSISOPTIONS-sl = /DGUITOOL=vdmgde.exe /DCLITOOL=vdmde.exe /DTOOLNAME=VDM-SL\ Toolbox /DVDMSL=1
NSISOPTIONS-pp = /DGUITOOL=vppgde.exe /DCLITOOL=vppde.exe /DTOOLNAME=VDM++\ Toolbox /DVDMPP=1
NSISOPTIONS-rt = /DGUITOOL=vicegde.exe /DCLITOOL=vicede.exe /DTOOLNAME=VDM++\ VICE\ Toolbox /DVICE=1

ifeq ($(strip $(CLCPUTYPE)),x64)
PPGUITOOL=vppqt.exe
RTGUITOOL=viceqt.exe
else
ifeq ($(strip $(INSTALLER)),NSIS)
PPGUITOOL=vppqt.exe
RTGUITOOL=viceqt.exe
else
PPGUITOOL=vppumlqt.exe
RTGUITOOL=viceumlqt.exe
endif
endif

ifdef SPLIT
WISESPLIT=-s
SETUPNAME=$*$$VERSION.exe
GETVERSION=$(shell ./print_name_vers -$(INSTVERSION-$*) --outputshortversion)
else
WISESPLIT=
SNAME=setup$*
#SETUPNAME=setup$*-$$VERSION-$(RDATE)-$(VSVER).exe
S2NAME=$(SNAME:%rt=%vice)
ifeq ($(strip $(CLCPUTYPE)),x64)
SETUPNAME=$(S2NAME:%rtl=%vicel)-$$VERSION-$(RDATE)-$(VSVER)-x64.exe
else
SETUPNAME=$(S2NAME:%rtl=%vicel)-$$VERSION-$(RDATE)-$(VSVER)-i586.exe
endif
GETVERSION=$(shell ./print_name_vers -$(INSTVERSION-$*) --outputversion)
endif

df inst_addin.exe: inst_addin.wse instid-pp.wse
	cp $< inst_tmp.wse
	$(WISE) /C /S inst_tmp.wse
	mv inst_tmp.exe $@

setupall:
	rm -rf tools-*.obj vdm-*.obj vdmde-*.obj
	$(MAKE) vdm_debug.lib CG_debug.lib
	$(MAKE) instcm
	$(MAKE) be_ip-sl.exe be_spec-sl.exe
	$(MAKE) testfiles
	$(MAKE) inst-powertest-files

setupcm:
	rm -rf tools-*.obj vdm-*.obj vdmde-*.obj
	$(MAKE) vdm_debug.lib CG_debug.lib
	$(MAKE) instcm
	$(MAKE) be_ip-sl.exe be_spec-sl.exe

instcm:
	rm -rf vdmde.exe vdmqt.exe
	$(MAKE) instsl
	rm -rf vppde.exe $(PPGUITOOL) j2vdll.so
	$(MAKE) instpp
	rm -rf vicede.exe $(RTGUITOOL) j2vdll.so
	$(MAKE) instrt

INSTFILES-sl = vdmde.exe vdmqt.exe
INSTFILES-pp = vppde.exe $(PPGUITOOL) j2vdll.so
INSTFILES-rt = vicede.exe $(RTGUITOOL) j2vdll.so

DISTDIR := distdir
clean-distdir: 
	rm -rf $(DISTDIR)

# instsl instpp instrt
ifeq ($(strip $(INSTALLER)),NSIS)
inst%:  inst-%.files print_name_vers.exe $(VDMMANIFEST)
	cp $(VPATH)/license/* .
	cp $(VPATH)/$(NSISSCR) .
	VERSION=$(GETVERSION); \
	NAME=$(SETUPNAME); \
	echo Creating $$NAME; \
	$(MAKENSIS) /V4 /DVERSION=$$VERSION /DVERSIONL=0.0.0.0 /DSETUPNAME=$$NAME $(NSISOPTIONS-$*) $(WIN64) $(NSISSCR)
else # must be WISE
inst%:  setup.wse instid-%.wse inst-%.files \
	print_name_vers.exe $(VDMMANIFEST)
	VERSION=$(GETVERSION); \
	NAME=$(SETUPNAME); \
	echo Creating $$NAME; \
	wisemod $(WISESPLIT) $(WISEDEF-$*) -n $$NAME   $< >tmp.wse
	cp instid-$(INSTVERSION-$*).wse instid.wse
	cp inst-$*.files inst.files
	$(WISE) /C /S tmp.wse
	rm -f tmp.wse
# instid.wse inst.files
#	rm -f tmp.wse 

WISE_SL_BIN = -d bin \
               -l "%MENUTITLE%" $(DISTDIR)/bin/vdmgde.exe \
               -l "%MENUTITLE% command line" $(DISTDIR)/bin/vdmde.exe

WISE_PP_BIN = -d bin \
                -l "%MENUTITLE%" $(DISTDIR)/bin/vppgde.exe \
                -l "%MENUTITLE% command line" $(DISTDIR)/bin/vppde.exe

WISE_RT_BIN = -d bin \
                -l "%MENUTITLE%" $(DISTDIR)/bin/vicegde.exe \
                -l "%MENUTITLE% command line" $(DISTDIR)/bin/vicede.exe

WISE_COMMON_OPTION = -d bin -s 0 -r $(DISTDIR)/dll \
	    -d stdlib -s 0 -r $(DISTDIR)/stdlib \
	    -d emacs -s 0 -r $(DISTDIR)/emacs \
	    -d examples -s 0 -r $(DISTDIR)/examples \
	    -d latex -s 0 -r $(DISTDIR)/latex \
	    -d word -s 0 -r $(DISTDIR)/word

ifdef QT_IMAGE_FILE
WISE_QM_FILES = -d $(QT_IMG) -s 0 -r $(DISTDIR)/$(QT_IMG)
else
WISE_QM_FILES =
endif

WISE_UML_OPTION = -d uml -s 0 -r $(DISTDIR)/uml

WISE_CG_OPTION = -d cg -s 0 -r $(DISTDIR)/cg

WISE_JAVACG_OPTION = -d javacg -s 0 -r $(DISTDIR)/javacg

WISE_CORBA_OPTION = -d api -s 0 -r $(DISTDIR)/api

WISE_J2V_OPTION = -d java2vdm -s 0 -r $(DISTDIR)/java2vdm

WISE_COMMON_OPTION_SL = -L instlinks.wse $(WISE_SL_BIN) \
		$(WISE_COMMON_OPTION) \
		$(WISE_QM_FILES)

WISE_COMMON_OPTION_PP = -L instlinks.wse $(WISE_PP_BIN) \
		$(WISE_COMMON_OPTION) \
		$(WISE_UML_OPTION) \
		$(WISE_QM_FILES)

WISE_COMMON_OPTION_RT = -L instlinks.wse $(WISE_RT_BIN) \
		$(WISE_COMMON_OPTION) \
		$(WISE_UML_OPTION) \
		$(WISE_QM_FILES)

WISE_OPTION_SL = $(WISE_COMMON_OPTION_SL) \
		 $(WISE_CG_OPTION) \
		 $(WISE_CORBA_OPTION)

WISE_OPTION_SLL = $(WISE_COMMON_OPTION_SL)

WISE_OPTION_PP = $(WISE_COMMON_OPTION_PP) \
		 $(WISE_CG_OPTION) \
		 $(WISE_CORBA_OPTION) \
		 $(WISE_JAVACG_OPTION) \
		 $(WISE_J2V_OPTION)

WISE_OPTION_PPL = $(WISE_COMMON_OPTION_PP)

WISE_OPTION_RT = $(WISE_COMMON_OPTION_RT) \
		 $(WISE_CG_OPTION) \
		 $(WISE_CORBA_OPTION) \
		 $(WISE_JAVACG_OPTION)

WISE_OPTION_RTL = $(WISE_COMMON_OPTION_RT)
endif

TESTFILES=metaiv.h m4lib_errmsg.h dlclass.h cg.h cg_aux.h random.h IO.java MATH.java VDMUtil.java \
IO_userimpl.cpp IO_userimpl-pp.cc IO_userimpl-sl.cc \
MATH_userimpl.cpp MATH_userimpl-pp.cc MATH_userimpl-sl.cc \
VDMUtil_userimpl.cpp VDMUtil_userimpl-pp.cc VDMUtil_userimpl-sl.cc

testfiles:
	mkdir -p testfiles
	for i in $(TESTFILES); do cp $(VPATH)/$$i testfiles/.; done

inst-sl.files: clean-distdir $(INSTFILES-sl) \
	inst-sl-stdlib inst-word inst-sl-latex inst-sl-examples inst-qt \
	inst-corbafiles inst-sl-cgfiles \
	inst-sl-emacs \
	inst-dll-sl
	mkdir -p $(DISTDIR)/bin
	cp vdmqt.exe $(DISTDIR)/bin/vdmgde.exe
	cp vdmde.exe $(DISTDIR)/bin/vdmde.exe
ifeq ($(strip $(INSTALLER)),WISE)
	dirtowise $(WISE_OPTION_SL) > $@ 
endif

inst-pp.files: clean-distdir $(INSTFILES-pp) \
	inst-pp-stdlib inst-word inst-pp-latex inst-pp-examples inst-qt inst-uml \
        inst-corbafiles inst-pp-cgfiles inst-javacg inst-java2vdm \
	inst-pp-emacs \
	inst-dll-pp
	mkdir -p $(DISTDIR)/bin
	cp $(PPGUITOOL) $(DISTDIR)/bin/vppgde.exe
	cp vppde.exe $(DISTDIR)/bin/vppde.exe
ifeq ($(strip $(INSTALLER)),WISE)
	cp j2vdll.so $(DISTDIR)/dll/.
	dirtowise $(WISE_OPTION_PP) > $@ 
else
	cp j2vdll.so $(DISTDIR)/bin/.
endif

inst-rt.files: clean-distdir $(INSTFILES-rt) \
	inst-pp-stdlib inst-word inst-pp-latex inst-pp-examples inst-qt inst-uml \
        inst-corbafiles inst-pp-cgfiles \
	inst-javacg inst-java2vdm \
	inst-rt-emacs \
	inst-dll-pp
	mkdir -p $(DISTDIR)/bin
	cp $(RTGUITOOL) $(DISTDIR)/bin/vicegde.exe
	cp vicede.exe $(DISTDIR)/bin/vicede.exe
ifeq ($(strip $(INSTALLER)),WISE)
	cp j2vdll.so $(DISTDIR)/dll/.
	dirtowise $(WISE_OPTION_RT) > $@ 
else
	cp j2vdll.so $(DISTDIR)/bin/.
endif

inst-dll-common: $(QMFILES)
ifeq ($(strip $(INSTALLER)),WISE)
	mkdir -p $(DISTDIR)/dll
	cp $(QT_DLL) $(DISTDIR)/dll
	cp $(QMFILES) $(DISTDIR)/dll
ifeq ($(strip $(QTVERSION)),4)
	mkdir $(DISTDIR)/dll/plugins
	mkdir $(DISTDIR)/dll/plugins/codecs
	cp $(QT_CODECS) $(DISTDIR)/dll/plugins/codecs/.
endif
else
	mkdir -p $(DISTDIR)/bin
	cp $(QT_DLL) $(DISTDIR)/bin
	cp $(QMFILES) $(DISTDIR)/bin
ifeq ($(strip $(QTVERSION)),4)
	mkdir $(DISTDIR)/bin/plugins
	mkdir $(DISTDIR)/bin/plugins/codecs
	cp $(QT_CODECS) $(DISTDIR)/bin/plugins/codecs/.
endif
endif

inst-dll-sl: inst-dll-common
ifeq ($(strip $(INSTALLER)),WISE)
	cp $(CORBA_DLL) $(DISTDIR)/dll
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/dll
endif
else
	cp $(CORBA_DLL) $(DISTDIR)/bin
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/bin
endif
endif

inst-dll-sl-lt: inst-dll-common
ifeq ($(strip $(INSTALLER)),WISE)
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/dll
endif
else
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/bin
endif
endif

inst-dll-pp: inst-dll-common
ifeq ($(strip $(INSTALLER)),WISE)
	cp $(CORBA_DLL) $(DISTDIR)/dll
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/dll
	cp $(WIN_MFC_DLL) $(DISTDIR)/dll
endif
else
	cp $(CORBA_DLL) $(DISTDIR)/bin
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/bin
endif
endif

inst-dll-pp-lt: inst-dll-common
ifeq ($(strip $(INSTALLER)),WISE)
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/dll
	cp $(WIN_MFC_DLL) $(DISTDIR)/dll
endif
else
ifneq ($(strip $(CLCPUTYPE)),x64)
	cp $(WIN_CRT_DLL) $(DISTDIR)/bin
endif
endif

instid-%.wse : print_name_vers$(EXE)
	$< -gui -$(INSTVERSION-$*) --outputwse > $@

endif  # win32 

ifeq ($(strip $(OSTYPE)),win32)
ifeq ($(strip $(INSTALLER)),NSIS)
inst-uml: NewClass.rtf VPPStereotypes.ini
	mkdir -p $(DISTDIR)/uml
	cp $^ $(DISTDIR)/uml
	mkdir -p $(DISTDIR)/bin
	cp $(XML_DIST_BIN) $(DISTDIR)/bin
else
inst-uml: NewClass.rtf VPPStereotypes.ini inst_addin.exe
	mkdir -p $(DISTDIR)/uml
	cp $^ $(DISTDIR)/uml
	mkdir -p $(DISTDIR)/dll
	cp $(XML_DIST_BIN) $(DISTDIR)/dll
endif
else
inst-uml: NewClass.rtf
	mkdir -p $(DISTDIR)/uml
	cp $^ $(DISTDIR)/uml
endif

# inst-corbafiles is share used both by window and unix dist* targets
ToolboxAPI.jar: Makefile.javaapi ToolboxClient.java
	$(MAKE) VPATH=$(VPATH) JDKHOME=$(JDKHOME) -f $< $@
	$(MAKE) VPATH=$(VPATH) JDKHOME=$(JDKHOME) -f $< ToolboxAPIDoc
inst-corbafiles: corba/Makefile corba/Makefile.win32 corba/client_example.cc \
                 corba/corba_client.cc corba/corba_client.h corba/metaiv_idl.idl \
                 corba/client_example.java corba/corba_api.idl ToolboxAPI.jar \
                 corba/runtest-pp corba/runtest-sl \
                 corba/runtest-pp-win corba/runtest-sl-win \
		 corba/javatest-pp corba/javatest-sl \
		 corba/javatest-pp-win corba/javatest-sl-win \
		 corba/javatest-pp-cygwin corba/javatest-sl-cygwin \
                 corba/nameservice corba/nameservice-win
	mkdir -p $(DISTDIR)/api/corba
	cp -r $^ $(DISTDIR)/api/corba
ifeq ($(strip $(OSTYPE)),win32)
	cp -r $(VPATH)/javaapi-doc $(DISTDIR)/api/corba
else
	cp -r javaapi-doc $(DISTDIR)/api/corba
endif

# bin.ARCH.tar.gz file. 

.PHONY: FAKE

inst-gcclibs:
	mkdir -p $(DISTDIR)/lib
	cp -dp $(CCPATH)../lib/*.* $(DISTDIR)/lib/.

ifeq ($(strip $(OSTYPE)),win32)
ifndef WINDEBUG
inst-cgfiles: $(CG_LIB_CG) $(CG_LIB_VDM) $(CG_FILES_INCLUDES)
else
inst-cgfiles: $(CG_LIB_CG) $(CG_LIB_VDM) $(CG_LIB_CG_DEBUG) $(CG_LIB_VDM_DEBUG) $(CG_FILES_INCLUDES)
endif
	mkdir -p $(patsubst %,$(DISTDIR)/%,$(CGDISTDIRS)) 
	cp $(patsubst %,$(VPATH)/%, $(CG_FILES_INCLUDES)) $(DISTDIR)/cg/include
	cp $(CG_LIB_CG) $(DISTDIR)/cg/lib/$(LIB_CG)
	cp $(CG_LIB_VDM) $(DISTDIR)/cg/lib/$(LIB_VDM)
ifndef WINDEBUG
	cp $(CG_LIB_CG_DEBUG) $(DISTDIR)/cg/lib/$(LIB_CG_WINDEBUG)
	cp $(CG_LIB_VDM_DEBUG) $(DISTDIR)/cg/lib/$(LIB_VDM_WINDEBUG)
endif
else
inst-cgfiles: $(CG_LIB_CG) $(CG_LIB_VDM) $(CG_FILES_INCLUDES)
	mkdir -p $(patsubst %,$(DISTDIR)/%,$(CGDISTDIRS)) 
	ln -s $(patsubst %,$(PWD)/%, $(CG_FILES_INCLUDES)) $(DISTDIR)/cg/include
	cp $(PWD)/$(CG_LIB_CG) $(DISTDIR)/cg/lib/$(LIB_CG)
	cp $(PWD)/$(CG_LIB_VDM) $(DISTDIR)/cg/lib/$(LIB_VDM)
ifeq ($(strip $(OSTYPE)),Darwin)
	ranlib $(DISTDIR)/cg/lib/$(LIB_CG) $(DISTDIR)/cg/lib/$(LIB_VDM)
endif
	$(STRIP) $(DISTDIR)/cg/lib/$(LIB_CG) $(DISTDIR)/cg/lib/$(LIB_VDM)
endif

ifeq ($(strip $(OSTYPE)),win32)
inst-pp-cgfiles: inst-cgfiles
	cp $(patsubst %,$(STDLIBDIR)/code/pp/%, $(STDLIB_CGFILES)) $(DISTDIR)/cg/include

inst-sl-cgfiles: inst-cgfiles
	cp $(patsubst %,$(STDLIBDIR)/code/sl/%, $(STDLIB_CGFILES)) $(DISTDIR)/cg/include
else
inst-pp-cgfiles: inst-cgfiles
	ln -s $(patsubst %,$(STDLIBDIR)/code/pp/%, $(STDLIB_CGFILES)) $(DISTDIR)/cg/include

inst-sl-cgfiles: inst-cgfiles
	ln -s $(patsubst %,$(STDLIBDIR)/code/sl/%, $(STDLIB_CGFILES)) $(DISTDIR)/cg/include
endif

inst-libs:
# Runtime Library
	mkdir -p $(DISTDIR)/lib
ifeq ($(strip $(OSTYPE)),Darwin)
	cp $(CORBA_DIST_LIB) $(DISTDIR)/lib/.

	install_name_tool -id $(NEWOMNIORB4) $(DISTDIR)/lib/$(LIBOMNIORB4)
	install_name_tool -id $(NEWOMNITHREAD) $(DISTDIR)/lib/$(LIBOMNITHREAD)
	install_name_tool -id $(NEWOMNIDYNAMIC4) $(DISTDIR)/lib/$(LIBOMNIDYNAMIC4)

	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(DISTDIR)/lib/$(LIBOMNIORB4)
	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(DISTDIR)/lib/$(LIBOMNIDYNAMIC4)
	install_name_tool -change $(OLDOMNIORB4) $(NEWOMNIORB4) $(DISTDIR)/lib/$(LIBOMNIDYNAMIC4)
endif
ifeq ($(strip $(OSTYPE)),Linux)
	cp $(CORBA_DIST_LIB) $(DISTDIR)/lib/.
	mkdir -p $(DISTDIR)/lib/linux
	cp $(CXXLIB) $(DISTDIR)/lib/linux/.
endif
ifeq ($(strip $(OSTYPE)),SunOS)
	cp $(CORBA_DIST_LIB) $(DISTDIR)/lib/.
endif

inst-pp-libs: inst-libs
ifeq ($(strip $(OSTYPE)),SunOS)
	cp $(ODBC_LIB) $(DISTDIR)/lib/.
endif

inst-sl-libs: inst-libs

inst-libs-lt:
# Runtime Library
	mkdir -p $(DISTDIR)/lib
ifeq ($(strip $(OSTYPE)),Linux)
	mkdir -p $(DISTDIR)/lib/linux
	cp $(CXXLIB) $(DISTDIR)/lib/linux.
endif

RDATE = $(shell LC_TIME=C date +%y%m%d)
# $$VERSION below is set before TARFILE{SL,PP,VICE} is referenced.
COMPLESSION=bzip2

ifeq ($(strip $(COMPLESSION)),gzip)
TARSUFFIX=.gz
TARCOMFLG=z
else
ifeq ($(strip $(COMPLESSION)),bzip2)
TARSUFFIX=.bz2
TARCOMFLG=j
endif
endif

DISTDIR = distdir
PWD = $(shell pwd)

CORBAFILES = Client.mk  \
             Client.nm  \
             client_example.cc  \
             corba_client.cc  \
             corba_client.h  \
             metaiv_idl.idl  \
             corba_api.idl     

EXAMPLESLFILES = sort.vdm sort.arg vdm_DefaultMod_implicit.cc \
               sort.vdm.ts sort_ex.cc DefaultMod.cc DefaultMod.h \
               Makefile sort.init sort.rtf sort-init.rtf vdmloop.bat \
	       vdmtest.bat Compare.txt compare.vdm end.txt equal.txt \
	       sort.arg.exp

ifeq ($(strip $(OSTYPE)),win32)
else
EXAMPLESLDIR = /home/vdm/doc/sort-ex
endif

EXAMPLEPPFILES=Makefile Makefile.winnt dosort.vpp gencode.eps \
		implsort.vpp inherit2.eps mergesort.vpp sort_pp.cc sorter.vpp \
		sortmachine.vpp vdm_ImplSort_implicit.cc explsort.vpp \
		DoSort.rtf ExplSort.rtf ImplSort.rtf MergeSort-init.rtf  \
		MergeSort.rtf SortMachine.rtf Sorter.rtf vdmloop.bat \
		vdmtest.bat compare.vpp Compare.txt sort.arg.exp end.txt \
		equal.txt sort.arg

ifeq ($(strip $(OSTYPE)),win32)
EXAMPLEPPDIR=//hermes/home/vdm/doc/sort-expp
else
EXAMPLEPPDIR=/home/vdm/doc/sort-expp
endif

#ALLDISTDIRS = bin emacs latex examples word lib stdlib
ALLDISTDIRS = bin lib
CGDISTDIRS = cg/lib cg/include
SLDISTDIRS =
PPDISTDIRS = javacg

SLDISTDIRS = 
EMACSSLFILES = vdmde.el COPYING
EMACSPPFILES = vppde.el COPYING
EMACSRTFILES = vicede.el COPYING

LATEXSLFILES = vdmsl-2e.sty vdmsl-2e-nms.sty
LATEXPPFILES = vdmsl-2e.sty vpp.sty vdmsl-2e-nms.sty vpp-nms.sty

ifeq ($(strip $(OSTYPE)),win32)
STDLIBDIR = $(VPATH)/speclink/stdlib-spec
LATEXDIR = $(VPATH)
EMACSDIR = $(VPATH)/baselink/elisp
WORDDIR = $(VPATH)/baselink/word
else
STDLIBDIR = $(PWD)/speclink/stdlib-spec
LATEXDIR = $(PWD)
EMACSDIR = $(PWD)/baselink/elisp
WORDDIR = $(PWD)/baselink/word
endif

WORDFILES = VDM.dot

# io files in the stdlib are not included in the release.
STDLIB_FILES_SL= math.vdm io.vdm VDMUtil.vdm
STDLIB_FILES_PP= math.vpp io.vpp VDMUtil.vpp

ifeq ($(strip $(OSTYPE)),win32)
STDLIB_CGFILES=IO_userimpl.cpp MATH_userimpl.cpp VDMUtil_userimpl.cpp
else
STDLIB_CGFILES=IO_userimpl.cc MATH_userimpl.cc VDMUtil_userimpl.cc
endif

ifeq ($(strip $(OSTYPE)),Linux)
CG_FILES_LIBS = libvdm.a libCG.a libvdm_shared.a libCG_shared.a
endif

ifeq ($(strip $(OSTYPE)),SunOS)
CG_FILES_LIBS = libvdm.a libCG.a libvdm_shared.a libCG_shared.a
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
CG_FILES_LIBS = libvdm.a libCG.a libvdm_shared.a libCG_shared.a
endif

ifeq ($(strip $(OSTYPE)),Darwin)
CG_FILES_LIBS = libvdm.a libCG.a
endif

CG_FILES_INCLUDES = metaiv.h cg.h cg_aux.h m4lib_errmsg.h dlclass.h random.h

ifeq ($(strip $(OSTYPE)),Darwin)
APPL_SL=vdmgde.app
APPL_PP=vppgde.app
APPL_RT=vicegde.app
CONT_SL=$(DISTDIR)/bin/$(APPL_SL)/Contents
CONT_PP=$(DISTDIR)/bin/$(APPL_PP)/Contents
CONT_RT=$(DISTDIR)/bin/$(APPL_RT)/Contents
endif

DISTDIRSL=vdmsl
DISTDIRPP=vdmpp
DISTDIRRT=vice

# This is a list of pair of FROM and LINKNAME directory names. 
distclean: 
	rm -rf $(DISTDIR) $(TARFILE)

distall:
	rm -rf tools-*.o vdm-*.o vdmde-*.o
	$(MAKE) distcm
	$(MAKE) be_ip-sl be_spec-sl

distcm:
	rm -rf vdmde vdmqt vppde vppqt vicede viceqt j2vdll.so
ifeq ($(strip $(GCC_MAJOR)),3)
	$(MAKE) contextinfo.h tb_version.h
	$(MAKE) NOOPT=1 libj2v-pp.a libj2v-rt.a
	$(MAKE) NOOPT=1 libparser-sl.a libparser-pp.a libparser-rt.a
endif
ifeq ($(strip $(OSTYPE)),Darwin)
	$(MAKE) $(LIB_VDM_FAT) $(LIB_CG_FAT)
endif
ifeq ($(strip $(OSTYPE)),Linux)
	$(MAKE) $(LIB_VDM_SHARED) $(LIB_CG_SHARED)
endif
ifeq ($(strip $(OSTYPE)),SunOS)
	$(MAKE) $(LIB_VDM_SHARED) $(LIB_CG_SHARED)
endif
ifeq ($(strip $(OSTYPE)),FreeBSD)
	$(MAKE) $(LIB_VDM_SHARED) $(LIB_CG_SHARED)
endif
	$(MAKE) convertToHexquad
	$(MAKE) distsl
	$(MAKE) distpp
	$(MAKE) distrt

VDMSL_CUI_NAME=vdmde
VDMSL_GUI_NAME=vdmqt
VDMPP_CUI_NAME=vppde
VDMPP_GUI_NAME=vppqt
VDMRT_CUI_NAME=vicede
VDMRT_GUI_NAME=viceqt
PRINT_NAME_VERS_NAME=print_name_vers

TBVERSION_SL=$(shell $(PRINT_NAME_VERS_NAME) -sl --outputversion)
TBVERSION_PP=$(shell $(PRINT_NAME_VERS_NAME) -pp --outputversion)
TBVERSION_RT=$(shell $(PRINT_NAME_VERS_NAME) -rt --outputversion)

TARFILESL = bin.$(DISTARCH).vdmsl-$(TBVERSION_SL)-$(RDATE).tar$(TARSUFFIX)
TARFILEPP = bin.$(DISTARCH).vdmpp-$(TBVERSION_PP)-$(RDATE).tar$(TARSUFFIX)
TARFILERT = bin.$(DISTARCH).vice-$(TBVERSION_RT)-$(RDATE).tar$(TARSUFFIX)

TARFILECG = bin.$(DISTARCH)-$(DISTCC).cglib-$(TBVERSION_PP)-$(RDATE).tar$(TARSUFFIX)

TBNAMESL=The VDM-SL Toolbox
TBNAMEPP=The VDM++ Toolbox
TBNAMERT=The VDM++ VICE Toolbox

MACPKGSL = $(TBNAMESL)-$(TBVERSION_SL)-$(RDATE)-$(OS_NAME).pkg
MACPKGPP = $(TBNAMEPP)-$(TBVERSION_PP)-$(RDATE)-$(OS_NAME).pkg
MACPKGRT = $(TBNAMERT)-$(TBVERSION_RT)-$(RDATE)-$(OS_NAME).pkg

MACPKGREFSL = jp.vdmtools.sl.pkg
MACPKGREFPP = jp.vdmtools.pp.pkg
MACPKGREFRT = jp.vdmtools.rt.pkg

MACPKGNAMESL = vdmsl.pkg
MACPKGNAMEPP = vdmpp.pkg
MACPKGNAMERT = vdmrt.pkg

MACDISTDIR=macdist

ifeq ($(strip $(OSTYPE)),win32)
CG_LIB_CG = CG.lib
CG_LIB_VDM = vdm.lib
CG_LIB_CG_DEBUG = CG_debug.lib
CG_LIB_VDM_DEBUG = vdm_debug.lib
LIB_CG = CG.lib
LIB_VDM = vdm.lib
LIB_CG_WINDEBUG = CG_debug.lib
LIB_VDM_WINDEBUG = vdm_debug.lib
else
ifeq ($(strip $(OSTYPE)),Darwin)
ifdef G3
CG_LIB_CG = panther/$(LIB_CG)
CG_LIB_VDM = panther/$(LIB_VDM)
else
CG_LIB_CG = $(LIB_CG_FAT)
CG_LIB_VDM = $(LIB_VDM_FAT)
endif
else
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
CG_LIB_CG = $(LIB_CG)
CG_LIB_VDM = $(LIB_VDM)
else
CG_LIB_CG = $(LIB_CG_SHARED)
CG_LIB_VDM = $(LIB_VDM_SHARED)
endif
endif
endif

#vdmde vdmgde libvdm.a libCG.a
distsl: $(VDMSL_CUI_NAME) $(VDMSL_GUI_NAME) $(PRINT_NAME_VERS_NAME) \
	$(QMFILES) \
	$(DISTDIR) \
	inst-sl-libs \
	inst-sl-stdlib inst-sl-examples \
	inst-sl-cgfiles inst-corbafiles \
	inst-qt inst-sl-emacs inst-sl-latex inst-word
	mkdir -p $(DISTDIR)/bin
ifeq ($(strip $(OSTYPE)),Darwin)
	cp $(PWD)/$(VDMSL_CUI_NAME) $(DISTDIR)/bin/vdmde
	$(STRIP) $(DISTDIR)/bin/vdmde
	mkdir -p $(CONT_SL)/MacOS $(CONT_SL)/Resources $(CONT_SL)/lib
	mkdir -p $(CONT_SL)/Resources/English.lproj
	mkdir -p $(CONT_SL)/Resources/Japanese.lproj
	cp $(PWD)/$(VDMSL_GUI_NAME) $(CONT_SL)/MacOS/vdmgde
	$(STRIP) $(CONT_SL)/MacOS/vdmgde
	cp $(PWD)/PkgInfo $(CONT_SL)/PkgInfo 
	cp $(PWD)/Info.plist.sl $(CONT_SL)/Info.plist
	cp $(PWD)/vdmtools.icns $(CONT_SL)/Resources/vdmtools.icns
# omniORB
	cp $(DISTDIR)/lib/$(LIBOMNIORB4) $(DISTDIR)/lib/$(LIBOMNITHREAD) $(DISTDIR)/lib/$(LIBOMNIDYNAMIC4) \
	   $(CONT_SL)/lib/.

	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(DISTDIR)/bin/vdmde
	install_name_tool -change $(OLDOMNIORB4) $(NEWOMNIORB4) $(DISTDIR)/bin/vdmde
	install_name_tool -change $(OLDOMNIDYNAMIC4) $(NEWOMNIDYNAMIC4) $(DISTDIR)/bin/vdmde

	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(CONT_SL)/MacOS/vdmgde
	install_name_tool -change $(OLDOMNIORB4) $(NEWOMNIORB4) $(CONT_SL)/MacOS/vdmgde
	install_name_tool -change $(OLDOMNIDYNAMIC4) $(NEWOMNIDYNAMIC4) $(CONT_SL)/MacOS/vdmgde
# Qt
	cp $(QMFILES) $(CONT_SL)/MacOS/.
ifeq ($(strip $(QTVERSION)),4)
ifdef COCOA 
	mkdir -p $(CONT_SL)/Frameworks
	mkdir -p $(CONT_SL)/Frameworks/QtGui.framework/Versions/4
	mkdir -p $(CONT_SL)/Frameworks/QtCore.framework/Versions/4

	cp $(OLDQTCORE) $(CONT_SL)/Frameworks/$(FWQTCORE)
	cp $(OLDQTGUI) $(CONT_SL)/Frameworks/$(FWQTGUI)
	cp -r $(QT4DIR)/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib $(CONT_SL)/Resources/.

	install_name_tool -id $(NEWQTCORE) $(CONT_SL)/Frameworks/$(FWQTCORE)
	install_name_tool -id $(NEWQTGUI) $(CONT_SL)/Frameworks/$(FWQTGUI)

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_SL)/Frameworks/$(FWQTGUI)
else
	cp -r $(QT_LIB) $(CONT_SL)/lib/.
	install_name_tool -id $(NEWQTCORE) $(CONT_SL)/lib/$(LIBQTCORE)
	install_name_tool -id $(NEWQTGUI) $(CONT_SL)/lib/$(LIBQTGUI)

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_SL)/lib/$(LIBQTGUI)
endif
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_SL)/MacOS/vdmgde
	install_name_tool -change $(OLDQTGUI) $(NEWQTGUI) $(CONT_SL)/MacOS/vdmgde

	mkdir -p $(CONT_SL)/MacOS/plugins
	mkdir -p $(CONT_SL)/MacOS/plugins/codecs
	cp $(QT_CODECS) $(CONT_SL)/MacOS/plugins/codecs/.

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_SL)/MacOS/plugins/codecs/$(LIBQCNCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_SL)/MacOS/plugins/codecs/$(LIBQJPCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_SL)/MacOS/plugins/codecs/$(LIBQKRCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_SL)/MacOS/plugins/codecs/$(LIBQTWCODECS)
else
	cp -r $(QT_LIB) $(CONT_SL)/lib/.
	install_name_tool -id $(NEWQTMT) $(CONT_SL)/lib/$(LIBQTMT)
	install_name_tool -id $(NEWQUI) $(CONT_SL)/lib/$(LIBQUI)

	install_name_tool -change $(OLDQTMT) $(NEWQTMT) $(CONT_SL)/lib/$(LIBQUI)

	install_name_tool -change $(OLDQTMT) $(NEWQTMT) $(CONT_SL)/MacOS/vdmgde
	install_name_tool -change $(OLDQUI) $(NEWQUI) $(CONT_SL)/MacOS/vdmgde
endif
ifdef QT_IMAGE_FILE
	mv $(DISTDIR)/$(QT_IMG) $(CONT_SL)/.
endif
else # Linux or Solaris or FreeBSD
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
	cp $(PWD)/$(VDMSL_CUI_NAME) $(DISTDIR)/bin/vdmde
	$(STRIP) $(DISTDIR)/bin/vdmde
	cp $(PWD)/$(VDMSL_GUI_NAME) $(DISTDIR)/bin/vdmqt
	$(STRIP) $(DISTDIR)/bin/vdmqt
	cp $(QMFILES) $(DISTDIR)/bin/.
	cp $(CORBA_DIST_BIN) $(DISTDIR)/bin/.
else
	cp $(PWD)/$(VDMSL_CUI_NAME) $(DISTDIR)/bin/vdmde.bin
	$(STRIP) $(DISTDIR)/bin/vdmde.bin
	cat $(PWD)/vdmtools.script | perl -nle 's/%%BINNAME%%/vdmde.bin/; print;' > $(DISTDIR)/bin/vdmde
	chmod 755 $(DISTDIR)/bin/vdmde
	cp $(PWD)/$(VDMSL_GUI_NAME) $(DISTDIR)/bin/vdmqt.bin
	$(STRIP) $(DISTDIR)/bin/vdmqt.bin
	cat $(PWD)/vdmtools.script | perl -nle 's/%%BINNAME%%/vdmqt.bin/; print;' > $(DISTDIR)/bin/vdmgde
	chmod 755 $(DISTDIR)/bin/vdmgde
	cp $(QMFILES) $(DISTDIR)/bin/.
ifeq ($(strip $(QTVERSION)),4)
ifeq ($(strip $(OSTYPE)),SunOS)
	cp $(QT_LIB) $(DISTDIR)/lib/.
	mkdir -p $(DISTDIR)/bin/plugins/codecs
	cp $(QT_CODECS) $(DISTDIR)/bin/plugins/codecs/.
endif
endif
endif
endif
	rm -rf $(DISTDIRSL)
	ln -s $(DISTDIR) $(DISTDIRSL)
	VERSION=$(shell ./$(PRINT_NAME_VERS_NAME) -sl --outputversion); \
	echo Creating $(TARFILESL); \
	tar -$(TARCOMFLG)cv$(TARFOLLOWLINKS)f $(TARFILESL) ./$(DISTDIRSL)
	rm -rf $(DISTDIRSL)
ifdef HASPKGBUILD
	rm -rf $(MACDISTDIR)
	mkdir $(MACDISTDIR)
	cat Distribution.xml | \
	perl -nle 's/%%TITLE%%/$(TBNAMESL)/; print;' | \
	perl -nle 's/%%VERSION%%/$(TBVERSION_SL)/; print;' | \
	perl -nle 's/%%PKGREF%%/$(MACPKGREFSL)/; print;' | \
	perl -nle 's/%%PKGNAME%%/$(MACPKGNAMESL)/; print;' > $(MACDISTDIR)/Distribution.xml
	cp -r Resources $(MACDISTDIR)/.
	mkdir  -p $(MACDISTDIR)/distroot/"$(TBNAMESL)"
	cp -r $(DISTDIR)/* $(MACDISTDIR)/distroot/"$(TBNAMESL)"
	cd $(MACDISTDIR)/distroot; ln -s ./"$(TBNAMESL)"/bin/$(APPL_SL) "$(TBNAMESL).app" 
	pkgbuild --root $(MACDISTDIR)/distroot --identifier $(MACPKGREFSL) --install-location /Applications \
	--version $(TBVERSION_SL) $(MACDISTDIR)/$(MACPKGNAMESL)
	productbuild --distribution $(MACDISTDIR)/Distribution.xml --package-path $(MACDISTDIR) \
	--resources $(MACDISTDIR)/Resources "$(MACPKGSL)"
else
ifdef HASPKGMAKER
	rm -rf "$(TBNAMESL)"
	mkdir "$(TBNAMESL)"
	mkdir "$(TBNAMESL)/$(TBNAMESL)"
	cp -r $(DISTDIR)/* "$(TBNAMESL)/$(TBNAMESL)"
	cd "$(TBNAMESL)"; ln -s ./"$(TBNAMESL)"/bin/$(APPL_SL) "$(TBNAMESL).app" 
#	/Developer/usr/bin/packagemaker --title "$(TBNAMESL)" --root "$(TBNAMESL)" \
#	--install-to /Applications --version $(TBVERSION_SL) --id $(MACPKGREFSL) \
#	--target $(OS_NAME) --out "$(MACPKGSL)" --verbose
endif
endif

#vppgde vppde libvdm.a libCG.a
distpp: $(VDMPP_CUI_NAME) $(VDMPP_GUI_NAME) j2vdll.so $(PRINT_NAME_VERS_NAME) \
	$(QMFILES) \
	$(DISTDIR) \
	inst-pp-libs \
	inst-pp-stdlib inst-pp-examples \
	inst-pp-cgfiles inst-corbafiles \
	inst-javacg inst-java2vdm javaapi.jar \
	inst-uml \
	inst-qt inst-pp-emacs inst-pp-latex inst-word
	mkdir -p $(DISTDIR)/bin
ifeq ($(strip $(OSTYPE)),Darwin)
	cp $(PWD)/$(VDMPP_CUI_NAME) $(DISTDIR)/bin/vppde
	$(STRIP) $(DISTDIR)/bin/vppde
	mkdir -p $(CONT_PP)/MacOS $(CONT_PP)/Resources $(CONT_PP)/lib $(CONT_PP)/uml
	mkdir -p $(CONT_PP)/Resources/English.lproj
	mkdir -p $(CONT_PP)/Resources/Japanese.lproj
	cp $(PWD)/$(VDMPP_GUI_NAME) $(CONT_PP)/MacOS/vppgde
	$(STRIP) $(CONT_PP)/MacOS/vppgde
	cp $(PWD)/PkgInfo $(CONT_PP)/PkgInfo 
	cp $(PWD)/Info.plist.pp $(CONT_PP)/Info.plist
	cp $(PWD)/vdmtools.icns $(CONT_PP)/Resources/vdmtools.icns
# UML
	cp $(PWD)/NewClass.rtf $(CONT_PP)/uml/.
# omniORB
	cp $(DISTDIR)/lib/$(LIBOMNIORB4) $(DISTDIR)/lib/$(LIBOMNITHREAD) $(DISTDIR)/lib/$(LIBOMNIDYNAMIC4) \
	   $(CONT_PP)/lib/.

	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(DISTDIR)/bin/vppde
	install_name_tool -change $(OLDOMNIORB4) $(NEWOMNIORB4) $(DISTDIR)/bin/vppde
	install_name_tool -change $(OLDOMNIDYNAMIC4) $(NEWOMNIDYNAMIC4) $(DISTDIR)/bin/vppde

	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(CONT_PP)/MacOS/vppgde
	install_name_tool -change $(OLDOMNIORB4) $(NEWOMNIORB4) $(CONT_PP)/MacOS/vppgde
	install_name_tool -change $(OLDOMNIDYNAMIC4) $(NEWOMNIDYNAMIC4) $(CONT_PP)/MacOS/vppgde
# Qt
	cp $(QMFILES) $(CONT_PP)/MacOS/.
ifeq ($(strip $(QTVERSION)),4)
ifdef COCOA 
	mkdir -p $(CONT_PP)/Frameworks
	mkdir -p $(CONT_PP)/Frameworks/QtGui.framework/Versions/4
	mkdir -p $(CONT_PP)/Frameworks/QtCore.framework/Versions/4

	cp $(OLDQTCORE) $(CONT_PP)/Frameworks/$(FWQTCORE)
	cp $(OLDQTGUI) $(CONT_PP)/Frameworks/$(FWQTGUI)
	cp -r $(QT4DIR)/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib $(CONT_PP)/Resources/.

	install_name_tool -id $(NEWQTCORE) $(CONT_PP)/Frameworks/$(FWQTCORE)
	install_name_tool -id $(NEWQTGUI) $(CONT_PP)/Frameworks/$(FWQTGUI)

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_PP)/Frameworks/$(FWQTGUI)
else
	cp -r $(QT_LIB) $(CONT_PP)/lib/.
	install_name_tool -id $(NEWQTCORE) $(CONT_PP)/lib/$(LIBQTCORE)
	install_name_tool -id $(NEWQTGUI) $(CONT_PP)/lib/$(LIBQTGUI)

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_PP)/lib/$(LIBQTGUI)
endif
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_PP)/MacOS/vppgde
	install_name_tool -change $(OLDQTGUI) $(NEWQTGUI) $(CONT_PP)/MacOS/vppgde

	mkdir -p $(CONT_PP)/MacOS/plugins
	mkdir -p $(CONT_PP)/MacOS/plugins/codecs
	cp $(QT_CODECS) $(CONT_PP)/MacOS/plugins/codecs/.

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_PP)/MacOS/plugins/codecs/$(LIBQCNCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_PP)/MacOS/plugins/codecs/$(LIBQJPCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_PP)/MacOS/plugins/codecs/$(LIBQKRCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_PP)/MacOS/plugins/codecs/$(LIBQTWCODECS)
else
	cp -r $(QT_LIB) $(CONT_PP)/lib/.
	install_name_tool -id $(NEWQTMT) $(CONT_PP)/lib/$(LIBQTMT)
	install_name_tool -id $(NEWQUI) $(CONT_PP)/lib/$(LIBQUI)

	install_name_tool -change $(OLDQTMT) $(NEWQTMT) $(CONT_PP)/lib/$(LIBQUI)

	install_name_tool -change $(OLDQTMT) $(NEWQTMT) $(CONT_PP)/MacOS/vppgde
	install_name_tool -change $(OLDQUI) $(NEWQUI) $(CONT_PP)/MacOS/vppgde
endif
ifdef QT_IMAGE_FILE
	mv $(DISTDIR)/$(QT_IMG) $(CONT_PP)/.
endif
else # Linux or Solaris or FreeBSD
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
	cp $(PWD)/$(VDMPP_CUI_NAME) $(DISTDIR)/bin/vppde
	$(STRIP) $(DISTDIR)/bin/vppde
	cp $(PWD)/$(VDMPP_GUI_NAME) $(DISTDIR)/bin/vppqt
	$(STRIP) $(DISTDIR)/bin/vppqt
	cp $(QMFILES) $(DISTDIR)/bin/.
	cp $(CORBA_DIST_BIN) $(DISTDIR)/bin/.
else
	cp $(PWD)/$(VDMPP_CUI_NAME) $(DISTDIR)/bin/vppde.bin
	$(STRIP) $(DISTDIR)/bin/vppde.bin
	cat $(PWD)/vdmtools.script | perl -nle 's/%%BINNAME%%/vppde.bin/; print;' > $(DISTDIR)/bin/vppde
	chmod 755 $(DISTDIR)/bin/vppde
	cp $(PWD)/$(VDMPP_GUI_NAME) $(DISTDIR)/bin/vppqt.bin
	$(STRIP) $(DISTDIR)/bin/vppqt.bin
	cat $(PWD)/vdmtools.script | perl -nle 's/%%BINNAME%%/vppqt.bin/; print;' > $(DISTDIR)/bin/vppgde
	chmod 755 $(DISTDIR)/bin/vppgde
	cp $(QMFILES) $(DISTDIR)/bin/.
ifeq ($(strip $(QTVERSION)),4)
ifeq ($(strip $(OSTYPE)),SunOS)
	cp $(QT_LIB) $(DISTDIR)/lib/.
	mkdir -p $(DISTDIR)/bin/plugins/codecs
	cp $(QT_CODECS) $(DISTDIR)/bin/plugins/codecs/.
endif
endif
endif
endif
	cp $(PWD)/j2vdll.so $(DISTDIR)/bin/j2vdll.so
	rm -rf $(DISTDIRPP)
	ln -s $(DISTDIR) $(DISTDIRPP)
	VERSION=$(shell ./$(PRINT_NAME_VERS_NAME) -pp --outputversion); \
	echo Creating $(TARFILEPP); \
	tar -$(TARCOMFLG)cv$(TARFOLLOWLINKS)f $(TARFILEPP) ./$(DISTDIRPP)
	rm -rf $(DISTDIRPP)
ifdef HASPKGBUILD
	rm -rf $(MACDISTDIR)
	mkdir $(MACDISTDIR)
	cat Distribution.xml | \
	perl -nle 's/%%TITLE%%/$(TBNAMEPP)/; print;' | \
	perl -nle 's/%%VERSION%%/$(TBVERSION_PP)/; print;' | \
	perl -nle 's/%%PKGREF%%/$(MACPKGREFPP)/; print;' | \
	perl -nle 's/%%PKGNAME%%/$(MACPKGNAMEPP)/; print;' > $(MACDISTDIR)/Distribution.xml
	cp -r Resources $(MACDISTDIR)/.
	mkdir -p $(MACDISTDIR)/distroot/"$(TBNAMEPP)"
	cp -r $(DISTDIR)/* $(MACDISTDIR)/distroot/"$(TBNAMEPP)"
	cd $(MACDISTDIR)/distroot; ln -s ./"$(TBNAMEPP)"/bin/$(APPL_PP) "$(TBNAMEPP).app" 
	pkgbuild --root $(MACDISTDIR)/distroot --identifier $(MACPKGREFPP) --install-location /Applications \
	--version $(TBVERSION_PP) $(MACDISTDIR)/$(MACPKGNAMEPP)
	productbuild --distribution $(MACDISTDIR)/Distribution.xml --package-path $(MACDISTDIR) \
	--resources $(MACDISTDIR)/Resources "$(MACPKGPP)"
else
ifdef HASPKGMAKER
	rm -rf "$(TBNAMEPP)"
	mkdir "$(TBNAMEPP)"
	mkdir "$(TBNAMEPP)/$(TBNAMEPP)"
	cp -r $(DISTDIR)/* "$(TBNAMEPP)/$(TBNAMEPP)"
	cd "$(TBNAMEPP)"; ln -s ./"$(TBNAMEPP)"/bin/$(APPL_PP) "$(TBNAMEPP).app" 
#	/Developer/usr/bin/packagemaker --title "$(TBNAMEPP)" --root "$(TBNAMEPP)" \
#	--install-to /Applications --version $(TBVERSION_PP) --id $(MACPKGREFPP) \
#	--target $(OS_NAME) --out "$(MACPKGPP)" --verbose
endif
endif

$(DISTDIR): FAKE
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)

distrt: $(VDMRT_CUI_NAME) $(VDMRT_GUI_NAME) j2vdll.so \
	$(QMFILES) \
	print_name_vers \
	$(DISTDIR)  \
	inst-pp-libs \
	inst-pp-stdlib inst-rt-examples \
	inst-pp-cgfiles inst-corbafiles \
	inst-javacg inst-java2vdm javaapi.jar \
	inst-uml \
	inst-qt inst-rt-emacs inst-pp-latex inst-word
	mkdir -p $(DISTDIR)/bin
ifeq ($(strip $(OSTYPE)),Darwin)
	cp $(PWD)/$(VDMRT_CUI_NAME) $(DISTDIR)/bin/vicede
	$(STRIP) $(DISTDIR)/bin/vicede
	mkdir -p $(CONT_RT)/MacOS $(CONT_RT)/Resources $(CONT_RT)/lib $(CONT_RT)/uml
	mkdir -p $(CONT_RT)/Resources/English.lproj
	mkdir -p $(CONT_RT)/Resources/Japanese.lproj
	cp $(PWD)/$(VDMRT_GUI_NAME) $(CONT_RT)/MacOS/vicegde
	$(STRIP) $(CONT_RT)/MacOS/vicegde
	cp $(PWD)/PkgInfo $(CONT_RT)/PkgInfo 
	cp $(PWD)/Info.plist.rt $(CONT_RT)/Info.plist
	cp $(PWD)/vdmtools.icns $(CONT_RT)/Resources/vdmtools.icns
# UML
	cp $(PWD)/NewClass.rtf $(CONT_RT)/uml/.
# omniORB
	cp $(DISTDIR)/lib/$(LIBOMNIORB4) $(DISTDIR)/lib/$(LIBOMNITHREAD) $(DISTDIR)/lib/$(LIBOMNIDYNAMIC4) \
	   $(CONT_RT)/lib/.

	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(DISTDIR)/bin/vicede
	install_name_tool -change $(OLDOMNIORB4) $(NEWOMNIORB4) $(DISTDIR)/bin/vicede
	install_name_tool -change $(OLDOMNIDYNAMIC4) $(NEWOMNIDYNAMIC4) $(DISTDIR)/bin/vicede

	install_name_tool -change $(OLDOMNITHREAD) $(NEWOMNITHREAD) $(CONT_RT)/MacOS/vicegde
	install_name_tool -change $(OLDOMNIORB4) $(NEWOMNIORB4) $(CONT_RT)/MacOS/vicegde
	install_name_tool -change $(OLDOMNIDYNAMIC4) $(NEWOMNIDYNAMIC4) $(CONT_RT)/MacOS/vicegde
# Qt
	cp $(QMFILES) $(CONT_RT)/MacOS/.
ifeq ($(strip $(QTVERSION)),4)
ifdef COCOA 
	mkdir -p $(CONT_RT)/Frameworks
	mkdir -p $(CONT_RT)/Frameworks/QtGui.framework/Versions/4
	mkdir -p $(CONT_RT)/Frameworks/QtCore.framework/Versions/4

	cp $(OLDQTCORE) $(CONT_RT)/Frameworks/$(FWQTCORE)
	cp $(OLDQTGUI) $(CONT_RT)/Frameworks/$(FWQTGUI)
	cp -r $(QT4DIR)/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib $(CONT_RT)/Resources/.

	install_name_tool -id $(NEWQTCORE) $(CONT_RT)/Frameworks/$(FWQTCORE)
	install_name_tool -id $(NEWQTGUI) $(CONT_RT)/Frameworks/$(FWQTGUI)

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_RT)/Frameworks/$(FWQTGUI)
else
	cp -r $(QT_LIB) $(CONT_RT)/lib/.
	install_name_tool -id $(NEWQTCORE) $(CONT_RT)/lib/$(LIBQTCORE)
	install_name_tool -id $(NEWQTGUI) $(CONT_RT)/lib/$(LIBQTGUI)

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_RT)/lib/$(LIBQTGUI)
endif
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_RT)/MacOS/vicegde
	install_name_tool -change $(OLDQTGUI) $(NEWQTGUI) $(CONT_RT)/MacOS/vicegde

	mkdir -p $(CONT_RT)/MacOS/plugins
	mkdir -p $(CONT_RT)/MacOS/plugins/codecs
	cp $(QT_CODECS) $(CONT_RT)/MacOS/plugins/codecs/.

	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_RT)/MacOS/plugins/codecs/$(LIBQCNCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_RT)/MacOS/plugins/codecs/$(LIBQJPCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_RT)/MacOS/plugins/codecs/$(LIBQKRCODECS)
	install_name_tool -change $(OLDQTCORE) $(NEWQTCORE) $(CONT_RT)/MacOS/plugins/codecs/$(LIBQTWCODECS)
else
	cp -r $(QT_LIB) $(CONT_RT)/lib/.
	install_name_tool -id $(NEWQTMT) $(CONT_RT)/lib/$(LIBQTMT)
	install_name_tool -id $(NEWQUI) $(CONT_RT)/lib/$(LIBQUI)

	install_name_tool -change $(OLDQTMT) $(NEWQTMT) $(CONT_RT)/lib/$(LIBQUI)

	install_name_tool -change $(OLDQTMT) $(NEWQTMT) $(CONT_RT)/MacOS/vicegde
	install_name_tool -change $(OLDQUI) $(NEWQUI) $(CONT_RT)/MacOS/vicegde
endif
ifdef QT_IMAGE_FILE
	mv $(DISTDIR)/$(QT_IMG) $(CONT_RT)/.
endif
else # Linux or Solaris or FreeBSD
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
	cp $(PWD)/$(VDMRT_CUI_NAME) $(DISTDIR)/bin/vicede
	$(STRIP) $(DISTDIR)/bin/vicede
	cp $(PWD)/$(VDMRT_GUI_NAME) $(DISTDIR)/bin/viceqt
	$(STRIP) $(DISTDIR)/bin/viceqt
	cp $(QMFILES) $(DISTDIR)/bin/.
	cp $(CORBA_DIST_BIN) $(DISTDIR)/bin/.
else
	cp $(PWD)/$(VDMRT_CUI_NAME) $(DISTDIR)/bin/vicede.bin
	$(STRIP) $(DISTDIR)/bin/vicede.bin
	cat $(PWD)/vdmtools.script | perl -nle 's/%%BINNAME%%/vicede.bin/; print;' > $(DISTDIR)/bin/vicede
	chmod 755 $(DISTDIR)/bin/vicede
	cp $(PWD)/$(VDMRT_GUI_NAME) $(DISTDIR)/bin/viceqt.bin
	$(STRIP) $(DISTDIR)/bin/viceqt.bin
	cat $(PWD)/vdmtools.script | perl -nle 's/%%BINNAME%%/viceqt.bin/; print;' > $(DISTDIR)/bin/vicegde
	chmod 755 $(DISTDIR)/bin/vicegde
	cp $(QMFILES) $(DISTDIR)/bin/.
ifeq ($(strip $(QTVERSION)),4)
ifeq ($(strip $(OSTYPE)),SunOS)
	cp $(QT_LIB) $(DISTDIR)/lib/.
	mkdir -p $(DISTDIR)/bin/plugins/codecs
	cp $(QT_CODECS) $(DISTDIR)/bin/plugins/codecs/.
endif
endif
endif
endif
	cp $(PWD)/j2vdll.so $(DISTDIR)/bin/j2vdll.so
	rm -rf $(DISTDIRRT)
	ln -s $(DISTDIR) $(DISTDIRRT)
	VERSION=$(shell ./$(PRINT_NAME_VERS_NAME) -rt --outputversion); \
	echo Creating $(TARFILERT); \
	tar -$(TARCOMFLG)cv$(TARFOLLOWLINKS)f $(TARFILERT) ./$(DISTDIRRT)
	rm -rf $(DISTDIRRT)
ifdef HASPKGBUILD
	rm -rf $(MACDISTDIR)
	mkdir $(MACDISTDIR)
	cat Distribution.xml | \
	perl -nle 's/%%TITLE%%/$(TBNAMERT)/; print;' | \
	perl -nle 's/%%VERSION%%/$(TBVERSION_RT)/; print;' | \
	perl -nle 's/%%PKGREF%%/$(MACPKGREFRT)/; print;' | \
	perl -nle 's/%%PKGNAME%%/$(MACPKGNAMERT)/; print;' > $(MACDISTDIR)/Distribution.xml
	cp -r Resources $(MACDISTDIR)/.
	mkdir -p $(MACDISTDIR)/distroot/"$(TBNAMERT)"
	cp -r $(DISTDIR)/* $(MACDISTDIR)/distroot/"$(TBNAMERT)"
	cd $(MACDISTDIR)/distroot; ln -s ./"$(TBNAMERT)"/bin/$(APPL_RT) "$(TBNAMERT).app" 
	pkgbuild --root $(MACDISTDIR)/distroot --identifier $(MACPKGREFRT) --install-location /Applications \
	--version $(TBVERSION_RT) $(MACDISTDIR)/$(MACPKGNAMERT)
	productbuild --distribution $(MACDISTDIR)/Distribution.xml --package-path $(MACDISTDIR) \
	--resources $(MACDISTDIR)/Resources "$(MACPKGRT)"
else
ifdef HASPKGMAKER
	rm -rf "$(TBNAMERT)"
	mkdir "$(TBNAMERT)"
	mkdir "$(TBNAMERT)/$(TBNAMERT)"
	cp -r $(DISTDIR)/* "$(TBNAMERT)/$(TBNAMERT)"
	cd "$(TBNAMERT)"; ln -s ./"$(TBNAMERT)"/bin/$(APPL_RT) "$(TBNAMERT).app" 
#	/Developer/usr/bin/packagemaker --title "$(TBNAMERT)" --root "$(TBNAMERT)" \
#	--install-to /Applications --version $(TBVERSION_RT) --id $(MACPKGREFRT) \
#	--target $(OS_NAME) --out "$(MACPKGRT)" --verbose
endif
endif

distcg: tb_version.h print_name_vers $(DISTDIR) $(CG_LIB_CG) $(CG_LIB_VDM) \
	inst-cgfiles
	VERSION=$(shell ./print_name_vers -pp --outputversion); \
	echo Creating $(TARFILECG); \
	tar -$(TARCOMFLG)cv$(TARFOLLOWLINKS)f $(TARFILECG) -C $(DISTDIR) .

########################################
# Distribution of source files for 
# libvdm.a and libCG.a 
########################################
distsrc: libvdm.src.tar.gz

libvdm.src.tar.gz: metaiv.cc metaiv.h cg.cc cg.h cg_aux.cc \
	vdm_malloc.cc vdm_fingerprint.cc vdm_recinfomap.cc \
	cg_aux.h tag.h vdm_priv.h m4lib_errmsg.cc m4lib_errmsg.h \
	vdm.make
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	cp $^ $(DISTDIR)
	mv $(DISTDIR)/vdm.make $(DISTDIR)/Makefile
	(cd $(DISTDIR); tar -zcvf $@ . )

# For OSS installation. 

EXCL = excl

codedist: code.tar.gz

code.tar.gz: FAKE
	-rm -f $@
	find ../code -type d -name CVS -printf "code/%P\n" >$(EXCL)
	find ../code -type d -name \.svn -printf "code/%P\n" >$(EXCL)
	tar -zcvfX $@ $(EXCL) -C .. code

TMP_TESTDIST = tmp_testdist

testdist: test.tar.gz

test.tar.gz: FAKE
	-rm -f $@ $(EXCL)
	-rm -rfv $(TMP_TESTDIST)
	mkdir $(TMP_TESTDIST) $(TMP_TESTDIST)/test $(TMP_TESTDIST)/test/cg \
              $(TMP_TESTDIST)/test/vdm
	ln -s /home/cg/test $(TMP_TESTDIST)/test/cg
	ln -s /home/vdm/test /home/vdm/fulltest $(TMP_TESTDIST)/test/vdm
	ln -s /home/vdm/ss-impl-check/tdallsl $(TMP_TESTDIST)/test
	-find $(TMP_TESTDIST) -follow -type d -name RCS -printf "%P\n" -prune > $(EXCL)
	tar -cv$(TARFOLLOWLINKS)fX - $(EXCL) -C $(TMP_TESTDIST) test | gzip > $@

ifeq ($(strip $(OSTYPE)),win32)
CLEANJAVALIB = 
else
CLEANJAVALIB = cleanjavadist cleanjavaapi
endif

clean: bin_clean distclean $(CLEANJAVALIB)
	rm -f *.a *.o *.obj
	rm -f *.output
	rm -f *.tab.cc *.tab.h *.tab.c
	rm -f *.qm
	rm -f DependGEN
	rm -f be-sl.cc
	rm -f tm.cc 
	rm -f date.cc 
	rm -f moc_*.cc 
	rm -f *.hh *SK.cc
	rm -f javaparse.y
	rm -f vdmpars.lex.c
	rm -f parssl.y pexprsl.y parssl.cc pexprsl.cc
	rm -f parspp.y pexprpp.y pstmtpp.y parspp.cc pexprpp.cc pstmtpp.cc
	rm -f parsrt.y pexprrt.y pstmtrt.y parsrt.cc pexprrt.cc pstmtrt.cc
	rm -f lex.backup
	rm -f unmangle.cc
	rm -f unmangle-sl.cc unmangle-pp.cc unmangle-rt.cc
	rm -f valueparser.cc valuescanner.cc
	rm -f javascan.cc javaparse.cc
	rm -f service_startup.cc lex.smd_.cc
	rm -f filter.pexpr
	rm -f lex.yy.c scanner-sl.cc filter.vdmpars
	rm -f filter.vppexpr filter.vpppars filter.vppstmt
	rm -f tmp-date.cc tmpscan.l 
	rm -f interface_processed.html preprocess.cc preprocessjojo.cc
	rm -f preprocess.l tagtab.cc tcl_embed.cc
	rm -f libparser-pp.a scanner-pp.cc
	rm -f priority.tab-pp.cc priority.tab-rt.cc 
	rm -f lex.tp_.cc lex.pf_.cc
	rm -f errmsg.cc tm-pp.cc tm-sl.cc contextinfo.cc contextinfo.h
	rm -f errmsg_jp.cc errmsg_jp_hex.txt
	rm -f convertToHexquad convertToHexquad.o
	rm -f contextinfo.stamp preprocess.h tb_version.h scanner-rt.cc
	rm -f be-pp.cc
	rm -f contextinfotest.cc
	rm -f contextinfo.stamp
	rm -f tb_version.cc tb_version.h
	rm -f tb_version-sl.cc tb_version-pp.cc tb_version-rt.cc
	rm -f metaiv_idl.stamp corba_api.stamp toolboxclient.stamp
	rm -f vdmprofile
	rm -f dlclass_test1.o dlclass_test1.so
	rm -f VDM.jar javaapi.jar ToolboxAPI.jar
	rm -rf jp libdoc javaapi javaapi-doc
ifeq ($(strip $(OSTYPE)),win32)
	rm -f *.lib *.ilk *.pdb *.files *.wse *.exp *.rch *.res *.map *.WS~
	rm -f *.manifest
	rm -f *.nsi gplv3.rtf
	rm -rf testfiles
endif

bin_clean:
	rm -f vdmde$(EXE) vdmqt$(EXE)
	rm -f vppde$(EXE) vppqt$(EXE) vppumlqt$(EXE)
	rm -f vicede$(EXE) viceqt$(EXE) viceumlqt$(EXE)
	rm -f be_spec-sl$(EXE) be_ip-sl$(EXE)
	rm -f print_name_vers$(EXE)
	rm -f j2vdll.so
	rm -f *.exe.manifest
	rm -f *a.fat
ifeq ($(strip $(OSTYPE)),win32)
	rm -f inst_addin.exe
endif

cleanlinks: 
	find . -type l -maxdepth 1 \( -name Makefile -o -name links -o -name lnconf -o -name baselink -o -print \) \
	  | perl -nle 'unlink $$_; print;'

-include Override.mak

# Variables that could have been overridden in Override.mak
dummy := $(foreach V, DEBUG OPTIMIZE PROFILING, \
  $(shell echo make is using $V=$($V) >/dev/tty) \
)
