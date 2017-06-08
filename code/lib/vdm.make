# Makefile for VDM PRODUCTS
#   libvdm.a and libCG.a
# 
# Copyright (C) Kyushu University

#
# $Id: vdm.make,v 1.2 1997/10/21 14:25:18 erik Exp $
#


RANLIB = ranlib 

LIB_OUTPUT_OPTION = -o $%

%.o: %.cc
	$(COMPILE.cc) $(SRCNAME)$< $(OUTPUT_OPTION)

%.o: %.c
	$(COMPILE.c) $(SRCNAME)$< $(OUTPUT_OPTION)

(%.o): %.cc
	$(COMPILE.cc) $(SRCNAME)$< $(LIB_OUTPUT_OPTION)
	$(AR) $(ARFLAGS) $@ $% 
	$(RM) $%

OPTIMIZE=-O
DEBUG = # -g

WARNINGS =   -fno-for-scope  -Wno-unused  -Wall
DEFINES = $(GLOBALDEFINES) $(ARCH_DEFINES)
GLOBALDEFINES = # -DSHOW_MALLOC_STATS -DVDM_FINGERPRINT

VDMTOOLSCFLAGS = $(SMALL) $(WARNINGS) $(OPTIMIZE) $(DEBUG)  -I. $(INCDIR)

CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++ 
CXX     = $(CCC)

# Notice:
# $(CFLAGS-$%) and $(CFLAGS-$@) is used for target specific flags, e.g., 
# with names like CFLAGS-vdmde.o 
# $(CFLAGS-$%) is applicable when libX.a(x.o) is being build because $% 
# expands to x.o. $@ expands to libX.a in this case, so with the define
# of CFLAGS below it is possible to both have a library and object specific
# defines. 
# $(CFLAGS-$@) is applicable when x.o is being built. In this case $% is empty.

CFLAGS    = $(VDMTOOLSCFLAGS) $(DEFINES) $(CFLAGS-$%) $(CFLAGS-$@) 
CCFLAGS   = $(CFLAGS)
CXXFLAGS  = $(CCFLAGS) $(TEMPLATE_FLAGS)

all: libvdm.a libCG.a

libvdm.a: libvdm.a(metaiv.o) \
          libvdm.a(m4lib_errmsg.o) \
          libvdm.a(pofstream.o) \
	  libvdm.a(position.o) 
	-$(RANLIB) $@

libvdm.a(position.o): position.h position.cc metaiv.h tag.h
libvdm.a(metaiv.o): metaiv.cc metaiv.h val_tag.h vdm_priv.h m4lib_errmsg.h
libvdm.a(pofstream.o): pofstream.cc pofstream.h
libvdm.a(m4lib_errmsg.o): m4lib_errmsg.cc m4lib_errmsg.h

#
# Library to be used with the generated code.
#


libCG.a(cg.o):  cg.h cg.cc metaiv.h
libCG.a(cg_aux.o): cg_aux.h cg_aux.cc metaiv.h 

libCG.a: libCG.a(cg.o) libCG.a(cg_aux.o) 
	-$(RANLIB) $@

